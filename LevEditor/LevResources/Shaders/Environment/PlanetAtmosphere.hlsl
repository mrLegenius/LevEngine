// The air between the camera and whatever it can see -- every pixel of it.
//
// This used to skip the pixels with nothing behind them and leave the sky to a baked cubemap. That
// cubemap is a picture of the sky from one position, and the halo around a planet's limb is a feature
// of that position, so it drifted off the planet as the camera flew and snapped back whenever it was
// rebuilt. Marching here instead costs a full screen of scattering and removes the possibility: there
// is no baked position left to go stale.
//
// The scattering is the same single-scattering model the light probe is built from -- see
// Atmosphere.hlsli -- integrated over the part of the view ray that lies inside the shell and in
// front of whatever stops it. Nothing here is a second atmosphere; it is the same one.

#include "../Quad.hlsl"
#include "Atmosphere.hlsli"

cbuffer PlanetAtmosphereParams : register(CB_PLANET_ATMOSPHERE)
{
    row_major matrix InverseViewProjection;

    float3 WorldCameraPosition;
    float ViewStepCount;

    float LightStepCount;
    float3 ParamsPadding;
};

Texture2D depthTexture : register(T_FOG_DEPTH);

float4 PSMain(PS_IN input) : SV_Target
{
    float depth = depthTexture.Load(int3(input.pos.xy, 0)).r;

    //<--- Nothing was drawn here, so the ray runs to the far side of the shell instead ---<<
    bool isSky = depth >= 0.9999f;

    float4 clip = float4(input.uv.x * 2.0f - 1.0f, (1.0f - input.uv.y) * 2.0f - 1.0f, depth, 1.0f);
    float4 world = mul(clip, InverseViewProjection);
    world /= world.w;

    float3 toSurface = world.xyz - WorldCameraPosition;
    float worldDistance = length(toSurface);
    if (worldDistance < 1e-4f)
        return 0.0f;

    float3 direction = toSurface / worldDistance;

    //<--- Into the atmosphere's own space, where the coefficients and the radii live ---<<
    float3 rayStart = (WorldCameraPosition - PlanetCenterWorld) * AtmosphereScale;
    float rayDistance = worldDistance * AtmosphereScale;

    float2 shell = RaySphereIntersect(rayStart, direction, AtmosphereRadius);
    if (shell.y < 0.0f)
        return 0.0f;

    // The segment that is both inside the shell and in front of the surface. Starting at the camera
    // when it is already inside is what makes this work from the ground as well as from orbit.
    float near = max(shell.x, 0.0f);
    float far = min(shell.y, rayDistance);

    if (isSky)
    {
        // No geometry to stop at, so the far side of the shell does -- except where the ray meets the
        // ground first. It can, right at the limb: the analytic sphere and the tessellated surface
        // disagree by a pixel there, and a ray let through marches below sea level, where the density
        // is an exponential of a negative height and runs away.
        far = shell.y;

        float2 ground = RaySphereIntersect(rayStart, direction, PlanetRadius);
        if (ground.x > 0.0f)
            far = min(far, ground.x);
    }

    if (far <= near)
        return 0.0f;

    int viewSteps = max(int(ViewStepCount), 2);
    int lightSteps = max(int(LightStepCount), 1);

    float3 transmittance;
    float3 scattering = IntegrateScattering(rayStart + direction * near, direction, far - near,
                                            viewSteps, lightSteps, transmittance) * SkyIntensity;

    // The blend is one source plus one minus source alpha, so the colour is added and the alpha is how
    // much of the scene the air swallows. Transmittance is per channel and alpha is one number, so this
    // takes its luminance: the reddening of a surface seen through a lot of air is then carried by the
    // in-scattering, which is per channel, rather than by the extinction. The alternative is dual
    // source blending, for a difference that only shows on a surface deep behind thick air.
    float extinction = 1.0f - dot(transmittance, float3(0.2126f, 0.7152f, 0.0722f));

    return float4(scattering, saturate(extinction));
}
