// What lies beyond the air, as seen by the camera: the skybox, the stars and the sun disks. Drawn as
// a cube centred on the camera, exactly like the static skybox it replaces.
//
// The air itself is deliberately not here any more. It used to be: this pass sampled a cubemap the
// precompute pass raymarched, which was cheap and smooth and refreshed as the sun moved. But a
// cubemap is a picture of the sky from one position, and a planet's limb is a feature *of that
// position* -- so as the camera flew the halo stayed where it had been baked, drifting off the planet
// and snapping back on every rebuild. Rebuilding often enough to hide it cost a 30ms stall roughly
// once a second, which is the same bug wearing a different hat: no threshold fixes both symptoms,
// it only picks which one you get.
//
// PlanetAtmospherePass now marches the air for every pixel on screen, sky included, and composites
// over whatever this pass left behind. The cubemap stays, but only as a light probe -- see
// EnvironmentPrecomputePass.

#include "Atmosphere.hlsli"
#include "StarField.hlsli"

//<--- Vertex Shader ---<<

cbuffer CameraConstantBuffer : register(CB_SKYBOX)
{
    row_major matrix viewProjection;
};

struct VS_IN
{
    float3 pos : POSITION;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 uv : UV;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output;

    // w = 0 drops the translation, so the cube stays centred on the camera and its vertices are
    // world space view directions.
    output.pos = mul(float4(input.pos, 0.0f), viewProjection);
    output.uv = input.pos;

    return output;
}

//<--- Pixel Shader ---<<

// The baked sky. Sampled only to judge how bright the sky is in this direction, never drawn.
TextureCube Cubemap;
SamplerState Sampler;

// What is behind the air. Black when the scene has no skybox, which composites to exactly the sky on
// its own -- so a scene without one renders what it always did.
TextureCube SkyboxCubemap;
SamplerState SkyboxSampler;

static const int k_DiskLightSteps = 6;

float4 PSMain(PS_IN input) : SV_Target
{
    float3 direction = normalize(input.uv);

    //<--- Everything beyond the air, at full brightness. The air is applied by the pass after ---<<
    float3 color = SkyboxCubemap.SampleLevel(SkyboxSampler, direction, 0).rgb;

    // Still sampled, but only as an estimate of how bright the sky is here -- a star is invisible
    // against a lit sky and that is the one thing the washout test needs to know. It is a light probe
    // now rather than a picture, so a second of lag in it cannot show.
    float4 sky = Cubemap.SampleLevel(Sampler, direction, 0);
    float3 skyRadiance = color * sky.a + sky.rgb;

    // How much of the sky one pixel covers. The star field needs it to know how wide to draw a point
    // source, and the derivatives are only available here in the pixel shader.
    float pixelAngle = max(length(ddx(direction)), length(ddy(direction)));

    color += CalcStarField(direction, pixelAngle, skyRadiance);

    //<--- Untransmitted: the atmosphere pass takes the air off everything in this target at once ---<<
    if (RenderSunDisks > 0.5f)
        color += CalcBodyDisks(direction, k_DiskLightSteps, false);

    return float4(color, 1.0f);
}
