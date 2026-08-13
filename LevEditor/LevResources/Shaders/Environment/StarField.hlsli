#ifndef LEV_STARFIELD_HLSLI
#define LEV_STARFIELD_HLSLI

// The night sky behind the atmosphere: individual stars and the galaxy they sit in.
//
// Two things hide a star, and both are modelled rather than dialled. The air, through the same
// transmittance the sun disks use, so the field reddens and dies towards the horizon. And the sky
// behind it: a star is seen by how far it stands out from its background, not by how much light it
// sends, which is why a first magnitude star is obvious at midnight and invisible at noon while
// emitting exactly the same. The eye handles that range by adapting; a renderer with a couple of stops
// of exposure cannot, so the contrast is applied here instead -- see CalcSkyWashout.
//
// Stars are placed by hashing, not stored, so the field costs no memory and is stable frame to frame:
// the sky is cut into cells on the six faces of a cube, and every cell either holds one star or does
// not, at a position decided by the hash of its own coordinates.
//
// Only the on-screen sky pass includes this. The cubemap feeding the image based lighting leaves it
// out: starlight contributes nothing a scene can see, and a 128 pixel face would smear the stars into
// noise anyway.

#include "Atmosphere.hlsli"

// Cells along the edge of a cube face at StarDensity 1. Six faces of 100x100 with a quarter of the
// cells occupied is around fifteen thousand stars, which is what a dark rural sky shows.
static const float k_StarGridResolution = 100.0f;
static const float k_StarOccupancy = 0.25f;

// Angular radius a star is drawn at when a pixel is smaller than this, in radians. Roughly the pixel
// of a 1080p viewport at 60 degrees, and the size the brightness below is calibrated against.
static const float k_StarReferenceRadius = 0.0006f;

// Radiance of the very brightest star in the field before StarBrightness scales it. Set against a
// moonless night sky, which this engine renders at a few thousandths, so the brightest stars carry
// real weight and the faintest sit right at the threshold the way they do outdoors.
static const float k_StarPeakRadiance = 2.0f;

// Sky radiance at which half the field is lost, relative to SkyIntensity so that turning the sky's
// exposure up or down does not change which stars survive. Roughly the sky under a full moon, which
// is about where a real sky starts costing you constellations.
static const float k_StarWashoutRadiance = 0.02f;

// Brightness of a star follows the magnitude scale, where counts climb steeply towards the faint end:
// the number of stars brighter than a given flux goes as flux to the -1.2, so a sky holds a handful of
// first magnitude stars and thousands sitting at the edge of visibility. Below are the faintest flux
// drawn and that exponent, which together decide how many bright stars a field ends up with -- about
// ten at full brightness and a hundred and forty above a tenth of it, which is what Earth has.
static const float k_StarFaintestFlux = 0.002f;
static const float k_StarCountExponent = 1.2f;

// Tilt of the galactic plane away from the celestial equator. On Earth it is about 60 degrees, which
// is why the Milky Way crosses the sky at an angle instead of following the stars' daily circles.
static const float k_GalacticTilt = 1.047f;

// Angular half width of the band, as a cosine offset from its plane. Around 8 degrees.
static const float k_MilkyWayWidth = 0.14f;

float3 StarHash(float3 p)
{
    p = frac(p * float3(0.1031f, 0.1030f, 0.0973f));
    p += dot(p, p.yxz + 33.33f);
    return frac((p.xxy + p.yxx) * p.zyx);
}

float StarValueNoise(float3 p)
{
    float3 cell = floor(p);
    float3 f = frac(p);
    f = f * f * (3.0f - 2.0f * f);

    float n000 = StarHash(cell + float3(0.0f, 0.0f, 0.0f)).x;
    float n100 = StarHash(cell + float3(1.0f, 0.0f, 0.0f)).x;
    float n010 = StarHash(cell + float3(0.0f, 1.0f, 0.0f)).x;
    float n110 = StarHash(cell + float3(1.0f, 1.0f, 0.0f)).x;
    float n001 = StarHash(cell + float3(0.0f, 0.0f, 1.0f)).x;
    float n101 = StarHash(cell + float3(1.0f, 0.0f, 1.0f)).x;
    float n011 = StarHash(cell + float3(0.0f, 1.0f, 1.0f)).x;
    float n111 = StarHash(cell + float3(1.0f, 1.0f, 1.0f)).x;

    return lerp(lerp(lerp(n000, n100, f.x), lerp(n010, n110, f.x), f.y),
                lerp(lerp(n001, n101, f.x), lerp(n011, n111, f.x), f.y), f.z);
}

float StarFbm(float3 p)
{
    return StarValueNoise(p) * 0.65f + StarValueNoise(p * 2.7f) * 0.35f;
}

float3 RotateAroundAxis(float3 target, float3 axis, float angle)
{
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    // Rodrigues' rotation.
    return target * cosAngle + cross(axis, target) * sinAngle + axis * dot(axis, target) * (1.0f - cosAngle);
}

// A direction as the face of a cube it points through and where on that face it lands. Only has to be
// a bijection the inverse below undoes -- it is a way of cutting the sphere into cells, not a texture
// lookup, so the uneven cell sizes towards the corners of a face do not matter.
void DirectionToFaceUV(float3 direction, out uint face, out float2 uv)
{
    float3 magnitude = abs(direction);

    if (magnitude.x >= magnitude.y && magnitude.x >= magnitude.z)
    {
        face = direction.x >= 0.0f ? 0 : 1;
        uv = direction.yz / magnitude.x;
    }
    else if (magnitude.y >= magnitude.z)
    {
        face = direction.y >= 0.0f ? 2 : 3;
        uv = direction.xz / magnitude.y;
    }
    else
    {
        face = direction.z >= 0.0f ? 4 : 5;
        uv = direction.xy / magnitude.z;
    }

    uv = uv * 0.5f + 0.5f;
}

float3 FaceUVToDirection(uint face, float2 uv)
{
    float2 onFace = uv * 2.0f - 1.0f;

    float3 direction;
    if (face == 0) direction = float3(1.0f, onFace.x, onFace.y);
    else if (face == 1) direction = float3(-1.0f, onFace.x, onFace.y);
    else if (face == 2) direction = float3(onFace.x, 1.0f, onFace.y);
    else if (face == 3) direction = float3(onFace.x, -1.0f, onFace.y);
    else if (face == 4) direction = float3(onFace.x, onFace.y, 1.0f);
    else direction = float3(onFace.x, onFace.y, -1.0f);

    return normalize(direction);
}

// Colour by surface temperature. Real stars run from cool red dwarfs through the yellow-white of a
// sun to blue supergiants, and the ones bright enough to see are mostly at the hot end of that.
float3 CalcStarColor(float temperature)
{
    float3 cool = float3(1.0f, 0.55f, 0.32f);
    float3 middle = float3(1.0f, 0.94f, 0.86f);
    float3 hot = float3(0.72f, 0.80f, 1.0f);

    return temperature < 0.5f
        ? lerp(cool, middle, temperature * 2.0f)
        : lerp(middle, hot, (temperature - 0.5f) * 2.0f);
}

// Axis the galactic plane stands perpendicular to, tilted off the celestial pole. Turns with the sky,
// so the band rises and sets along with the constellations in it.
float3 CalcGalacticPole()
{
    float3 sideways = abs(StarPole.y) < 0.9f
        ? normalize(cross(StarPole, float3(0.0f, 1.0f, 0.0f)))
        : normalize(cross(StarPole, float3(1.0f, 0.0f, 0.0f)));

    return normalize(StarPole * cos(k_GalacticTilt) + sideways * sin(k_GalacticTilt));
}

// How much of the galaxy lies along this direction: bright where the line of sight runs down the
// plane of the disc, and broken up by the dust clouds standing in front of it.
float CalcMilkyWay(float3 celestialDirection, float3 galacticPole)
{
    float distanceFromPlane = dot(celestialDirection, galacticPole) / k_MilkyWayWidth;
    float band = exp(-distanceFromPlane * distanceFromPlane);

    // Clumps of unresolved stars, then the dark lanes of dust cut back out of them.
    float clouds = 0.45f + 0.55f * StarFbm(celestialDirection * 6.0f);
    float dust = 1.0f - 0.65f * smoothstep(0.45f, 0.85f, StarFbm(celestialDirection * 2.3f + 11.0f));

    return band * clouds * dust;
}

// How much of the field survives the sky behind it. Contrast, not brightness, decides whether a star
// is visible: the brighter the background, the more a star needs to send to be picked out of it. This
// is the Weber form of that -- unity against a black sky, halved at the washout radiance above, and
// falling off inversely from there, so a daylit sky at a hundred times that level buries the field
// however high StarBrightness is turned.
float CalcSkyWashout(float3 skyRadiance)
{
    float background = dot(skyRadiance, float3(0.2126f, 0.7152f, 0.0722f));
    float threshold = k_StarWashoutRadiance * max(SkyIntensity, 1e-4f);

    return threshold / (threshold + max(background, 0.0f));
}

// pixelAngle is the angle a screen pixel covers, which the caller gets from the derivatives of the
// view direction. skyRadiance is what the sky itself is putting out in this direction, which is what
// decides how much of the field can be made out against it. Stars are far too small to resolve, so what is constant about one is the light it
// delivers, not how big it looks: the disk is drawn at least a pixel across and its radiance is
// divided by the area, which keeps a star equally bright at any resolution and stops it flickering
// as it slides between pixels.
float3 CalcStarField(float3 viewDirection, float pixelAngle, float3 skyRadiance)
{
    if (RenderStars < 0.5f) return 0.0f;

    float washout = CalcSkyWashout(skyRadiance);
    if (washout < 0.001f) return 0.0f;

    float3 rayStart = float3(0.0f, PlanetRadius + max(ViewHeight, 0.001f), 0.0f);

    // Below the horizon there is a planet in the way.
    float2 groundHit = RaySphereIntersect(rayStart, viewDirection, PlanetRadius);
    if (groundHit.x > 0.0f) return 0.0f;

    // So is a moon. Everything here lies far beyond every body in the sky, so a disk covers it: what
    // still shows in front of that disk is the scattered sky, which is why a moon in daylight looks
    // washed out rather than cut out of the sky.
    for (uint blockerIndex = 0; blockerIndex < BodyCount; blockerIndex++)
    {
        AtmosphereBody blocker = Bodies[blockerIndex];
        if (dot(viewDirection, blocker.Direction) >= blocker.CosAngularRadius) return 0.0f;
    }

    // Into the frame the stars are fixed in, which the sky has turned StarRotation away from.
    float3 celestialDirection = RotateAroundAxis(viewDirection, StarPole, -StarRotation);

    float3 galacticPole = CalcGalacticPole();
    float milkyWay = MilkyWayBrightness > 0.0f ? CalcMilkyWay(celestialDirection, galacticPole) : 0.0f;

    uint face;
    float2 uv;
    DirectionToFaceUV(celestialDirection, face, uv);

    float resolution = max(k_StarGridResolution * sqrt(StarDensity), 1.0f);
    float2 cell = floor(uv * resolution);

    float3 cellHash = StarHash(float3(cell, face));
    float3 starHash = StarHash(float3(cell, face) + 7.77f);

    // Stars crowd towards the plane of the galaxy, which is most of why the band looks like a band.
    float occupancy = k_StarOccupancy * (1.0f + 2.5f * milkyWay);

    float3 color = 0.0f;
    if (cellHash.x < occupancy)
    {
        // Kept off the edges of its cell so its whole disk stays inside, which is what lets a single
        // cell be tested instead of a neighbourhood.
        float2 starUV = (cell + 0.5f + (cellHash.yz - 0.5f) * 0.7f) / resolution;
        float3 starDirection = FaceUVToDirection(face, starUV);

        // Drawn from that power law by inverting it, so the shape of the distribution is the sky's
        // rather than whatever a pow() of a random number happens to give.
        float flux = min(k_StarFaintestFlux / pow(max(starHash.x, 1e-4f), 1.0f / k_StarCountExponent),
                         1.0f);

        // The brightest stars in the sky are mostly hot ones, so temperature leans on brightness.
        float temperature = saturate(pow(starHash.y, 0.45f) * 0.8f + flux * 0.3f);

        float radius = max(pixelAngle * 1.5f, k_StarReferenceRadius);
        float areaFalloff = (k_StarReferenceRadius * k_StarReferenceRadius) / (radius * radius);

        float offset = length(celestialDirection - starDirection) / radius;
        float profile = exp(-offset * offset);

        // Scintillation. The line of sight crosses more and more air towards the horizon, so a star
        // low in the sky boils while one overhead sits still.
        float altitude = max(viewDirection.y, 0.02f);
        float airMass = 1.0f / altitude;
        float twinkleAmount = StarTwinkle * saturate((airMass - 1.0f) * 0.25f);
        float twinklePhase = starHash.z * 6.2831853f;
        float twinkle = 1.0f + twinkleAmount * sin(SkyTime * 7.0f + twinklePhase)
            * sin(SkyTime * 3.3f + twinklePhase * 2.0f);

        color += CalcStarColor(temperature) * (flux * k_StarPeakRadiance * areaFalloff * profile
            * max(twinkle, 0.0f));
    }

    // Warm white: the band is the combined light of stars of every kind, weighted towards the old
    // yellow ones that make up the bulk of the disc.
    color += float3(1.0f, 0.97f, 0.92f) * milkyWay * MilkyWayBrightness * 0.04f;

    if (all(color <= 0.0f)) return 0.0f;

    // The same air the sun disks are seen through, which reddens and finally swallows anything low.
    return color * StarBrightness * SkyIntensity * washout
        * CalcTransmittance(rayStart, viewDirection, 4);
}

#endif
