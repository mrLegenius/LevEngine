#ifndef LEV_ATMOSPHERE_HLSLI
#define LEV_ATMOSPHERE_HLSLI

// Single scattering sky model. The atmosphere is two exponential layers -- molecules (Rayleigh)
// and aerosols (Mie) -- plus a tent shaped absorbing layer standing in for ozone. Light is
// integrated along the view ray, and at every step along a second ray towards each sun.
//
// Everything here is in kilometres, and every coefficient is an extinction rate in 1/km, so the
// same code renders any planet by swapping the numbers in the constant buffer.
//
// Both the on-screen sky and the cubemap used for image based lighting go through RenderSky(),
// only with different step counts, so the two can never drift apart.

#include "../Registers.hlsli"

#define MAX_BODIES 4

// A star or a moon. Both are just a direction and a radiance as far as the scattering is concerned;
// a moon differs only in that its radiance was borrowed from a star, which gives it a phase.
struct AtmosphereBody
{
    float3 Direction;
    float Intensity;

    float3 Color;
    float CosAngularRadius;

    float3 DirectionToStar;
    float PhaseFactor;

    float PhaseInfluence;
    float DiskBrightness;
    float2 BodyPadding;
};

cbuffer AtmosphereConstantBuffer : register(CB_ATMOSPHERE)
{
    float3 RayleighScattering;
    float RayleighScaleHeight;

    float3 AbsorptionCoefficients;
    float MieScattering;

    float MieAbsorption;
    float MieScaleHeight;
    float MieAnisotropy;
    float PlanetRadius;

    float AtmosphereRadius;
    float AbsorptionLayerCenter;
    float AbsorptionLayerWidth;
    float SkyIntensity;

    float3 GroundColor;
    float ViewHeight;

    uint BodyCount;
    float RenderSunDisks;
    float2 AtmospherePadding;

    float3 StarPole;
    float StarRotation;

    float RenderStars;
    float StarBrightness;
    float StarDensity;
    float StarTwinkle;

    float MilkyWayBrightness;
    float SkyTime;
    float2 StarPadding;

    AtmosphereBody Bodies[MAX_BODIES];
};

static const float k_AtmospherePi = 3.14159265359f;

// Near and far distance along a ray to a sphere centred on the origin. y < 0 means a miss.
float2 RaySphereIntersect(float3 origin, float3 direction, float radius)
{
    float b = dot(origin, direction);
    float c = dot(origin, origin) - radius * radius;
    float discriminant = b * b - c;

    if (discriminant < 0.0f)
        return float2(-1.0f, -1.0f);

    float sqrtDiscriminant = sqrt(discriminant);
    return float2(-b - sqrtDiscriminant, -b + sqrtDiscriminant);
}

// Relative density of each of the three layers at a given height above sea level.
float3 SampleDensity(float height)
{
    float rayleigh = exp(-height / RayleighScaleHeight);
    float mie = exp(-height / MieScaleHeight);
    float absorption = max(0.0f, 1.0f - abs(height - AbsorptionLayerCenter) / AbsorptionLayerWidth);

    return float3(rayleigh, mie, absorption);
}

float3 CalcExtinction(float3 densities)
{
    return RayleighScattering * densities.x
        + (MieScattering + MieAbsorption) * densities.y
        + AbsorptionCoefficients * densities.z;
}

float RayleighPhase(float cosTheta)
{
    return 3.0f / (16.0f * k_AtmospherePi) * (1.0f + cosTheta * cosTheta);
}

// Henyey-Greenstein. g pushes the lobe forward, which is what puts the bright halo around the sun.
float MiePhase(float cosTheta, float g)
{
    float g2 = g * g;
    float denominator = 1.0f + g2 - 2.0f * g * cosTheta;
    return (1.0f - g2) / (4.0f * k_AtmospherePi * max(denominator, 0.0001f) * sqrt(max(denominator, 0.0001f)));
}

// Fraction of light surviving from a point out to the edge of the atmosphere.
float3 CalcTransmittance(float3 position, float3 direction, int steps)
{
    float2 atmosphereHit = RaySphereIntersect(position, direction, AtmosphereRadius);
    if (atmosphereHit.y < 0.0f) return 1.0f;

    float stepSize = atmosphereHit.y / steps;

    float3 opticalDepth = 0.0f;
    for (int i = 0; i < steps; i++)
    {
        float3 samplePosition = position + direction * ((i + 0.5f) * stepSize);
        float height = length(samplePosition) - PlanetRadius;

        opticalDepth += CalcExtinction(SampleDensity(height)) * stepSize;
    }

    return exp(-opticalDepth);
}

// Light scattered into the view ray between rayStart and rayStart + rayDirection * rayLength.
// viewTransmittance comes back as how much of whatever is behind the segment still gets through.
float3 IntegrateScattering(float3 rayStart, float3 rayDirection, float rayLength,
                           int viewSteps, int lightSteps, out float3 viewTransmittance)
{
    float stepSize = rayLength / viewSteps;

    float3 opticalDepth = 0.0f;
    float3 inScattering = 0.0f;

    for (int step = 0; step < viewSteps; step++)
    {
        float3 samplePosition = rayStart + rayDirection * ((step + 0.5f) * stepSize);
        float height = length(samplePosition) - PlanetRadius;

        float3 densities = SampleDensity(height);
        float3 stepExtinction = CalcExtinction(densities) * stepSize;

        // Transmittance from the camera up to the middle of this step. Using the midpoint keeps
        // the first step from being counted as fully unattenuated.
        float3 transmittanceToSample = exp(-(opticalDepth + stepExtinction * 0.5f));

        for (uint bodyIndex = 0; bodyIndex < BodyCount; bodyIndex++)
        {
            AtmosphereBody body = Bodies[bodyIndex];

            // A sample in the planet's shadow receives nothing directly.
            float2 groundHit = RaySphereIntersect(samplePosition, body.Direction, PlanetRadius);
            if (groundHit.y > 0.0f && groundHit.x > 0.0f) continue;

            float3 bodyTransmittance = CalcTransmittance(samplePosition, body.Direction, lightSteps);

            float cosTheta = dot(rayDirection, body.Direction);
            float3 scattered = RayleighScattering * densities.x * RayleighPhase(cosTheta)
                + MieScattering * densities.y * MiePhase(cosTheta, MieAnisotropy);

            inScattering += transmittanceToSample * bodyTransmittance * scattered
                * body.Color * body.Intensity * body.PhaseFactor * stepSize;
        }

        opticalDepth += stepExtinction;
    }

    viewTransmittance = exp(-opticalDepth);
    return inScattering;
}

// Shading of a reflective disk at the pixel looking along viewDirection. The disk is the visible
// half of a sphere, so the surface normal can be recovered from how far the pixel sits from the
// centre -- and lighting that normal by the star is what draws the crescent.
float CalcPhaseShading(float3 viewDirection, AtmosphereBody body)
{
    // Offset from the centre of the disk, as a fraction of its radius.
    float3 offset = viewDirection - body.Direction * dot(viewDirection, body.Direction);
    float offsetLength = length(offset);
    float sinAngularRadius = sqrt(max(1.0f - body.CosAngularRadius * body.CosAngularRadius, 1e-12f));

    float radialFraction = saturate(offsetLength / sinAngularRadius);
    float3 tangent = offsetLength > 1e-8f ? offset / offsetLength : float3(0.0f, 0.0f, 0.0f);

    // Straight back at the viewer in the middle of the disk, perpendicular to the view at the rim.
    float3 normal = -body.Direction * sqrt(saturate(1.0f - radialFraction * radialFraction))
        + tangent * radialFraction;

    return lerp(1.0f, saturate(dot(normal, body.DirectionToStar)), body.PhaseInfluence);
}

// The disks alone, attenuated by the air between them and the camera. Kept separate from the sky
// because the sky is sampled from a cubemap while the disks stay analytic and sharp.
float3 CalcBodyDisks(float3 viewDirection, int lightSteps)
{
    float3 rayStart = float3(0.0f, PlanetRadius + max(ViewHeight, 0.001f), 0.0f);

    // Looking at the ground means no sun, whatever the direction says.
    float2 groundHit = RaySphereIntersect(rayStart, viewDirection, PlanetRadius);
    if (groundHit.x > 0.0f) return 0.0f;

    float3 color = 0.0f;
    for (uint bodyIndex = 0; bodyIndex < BodyCount; bodyIndex++)
    {
        AtmosphereBody body = Bodies[bodyIndex];

        float cosTheta = dot(viewDirection, body.Direction);
        if (cosTheta < body.CosAngularRadius) continue;

        // Limb darkening: the disk is dimmer towards its edge, which also softens the
        // one-pixel step the hard cutoff would otherwise leave.
        float edge = saturate((cosTheta - body.CosAngularRadius) / max(1.0f - body.CosAngularRadius, 1e-6f));
        float shading = 0.6f + 0.4f * sqrt(edge);

        // A body lit by a star is shaded by that star instead, so it shows a phase rather than a
        // uniformly bright disk. Its colour already holds the fully lit radiance.
        if (dot(body.DirectionToStar, body.DirectionToStar) > 0.0f)
            shading = CalcPhaseShading(viewDirection, body) * (0.85f + 0.15f * sqrt(edge));

        color += CalcTransmittance(rayStart, viewDirection, lightSteps)
            * body.Color * body.Intensity * body.DiskBrightness * shading;
    }

    return color * SkyIntensity;
}

// Radiance arriving from a direction, including the ground when looking down. Sun disks are opt in:
// the light probe leaves them out, because the same suns already light the scene directly and
// would otherwise be counted twice.
float3 RenderSky(float3 viewDirection, int viewSteps, int lightSteps, bool includeSunDisks)
{
    float3 rayStart = float3(0.0f, PlanetRadius + max(ViewHeight, 0.001f), 0.0f);

    float2 atmosphereHit = RaySphereIntersect(rayStart, viewDirection, AtmosphereRadius);
    if (atmosphereHit.y < 0.0f) return 0.0f;

    float rayStartDistance = max(atmosphereHit.x, 0.0f);
    float rayLength = atmosphereHit.y - rayStartDistance;

    float2 groundHit = RaySphereIntersect(rayStart, viewDirection, PlanetRadius);
    bool hitsGround = groundHit.x > 0.0f;
    if (hitsGround)
        rayLength = groundHit.x - rayStartDistance;

    rayStart += viewDirection * rayStartDistance;

    float3 viewTransmittance;
    float3 color = IntegrateScattering(rayStart, viewDirection, rayLength, viewSteps, lightSteps, viewTransmittance);

    if (hitsGround)
    {
        // Flat lambert ground so the lower hemisphere of the light probe carries a bounce colour
        // instead of being black.
        float3 groundPosition = rayStart + viewDirection * rayLength;
        float3 groundNormal = normalize(groundPosition);

        float3 groundLight = 0.0f;
        for (uint bodyIndex = 0; bodyIndex < BodyCount; bodyIndex++)
        {
            AtmosphereBody body = Bodies[bodyIndex];
            float3 bodyTransmittance = CalcTransmittance(groundPosition, body.Direction, lightSteps);

            groundLight += saturate(dot(groundNormal, body.Direction))
                * body.Color * body.Intensity * body.PhaseFactor * bodyTransmittance;
        }

        color += viewTransmittance * GroundColor * groundLight / k_AtmospherePi;
    }

    color *= SkyIntensity;

    if (!hitsGround && includeSunDisks && RenderSunDisks > 0.5f)
        color += CalcBodyDisks(viewDirection, lightSteps);

    return color;
}

#endif
