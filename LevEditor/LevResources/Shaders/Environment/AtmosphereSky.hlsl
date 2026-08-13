// The sky as seen by the camera. Drawn as a cube centred on the camera, exactly like the static
// skybox it replaces.
//
// The smooth part of the sky comes from the cubemap the precompute pass raymarches, which is
// cheap to sample and is refreshed as the sun moves. Only the sun disks are computed here, since
// they are the one feature a 128 pixel cube face cannot hold, and they cost nothing outside the
// handful of pixels they cover.

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

TextureCube Cubemap;
SamplerState Sampler;

static const int k_DiskLightSteps = 6;

float4 PSMain(PS_IN input) : SV_Target
{
    float3 direction = normalize(input.uv);

    float3 color = Cubemap.SampleLevel(Sampler, direction, 0).rgb;

    // How much of the sky one pixel covers. The star field needs it to know how wide to draw a point
    // source, and the derivatives are only available here in the pixel shader.
    float pixelAngle = max(length(ddx(direction)), length(ddy(direction)));

    // The sky is handed over as well: what a star has to stand out against is what decides whether it
    // can be seen at all.
    color += CalcStarField(direction, pixelAngle, color);

    if (RenderSunDisks > 0.5f)
        color += CalcBodyDisks(direction, k_DiskLightSteps);

    return float4(color, 1.0f);
}
