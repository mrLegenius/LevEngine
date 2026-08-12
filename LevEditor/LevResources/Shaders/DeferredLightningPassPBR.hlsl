// PBRCommon.hlsl, not MaterialPBR.hlsl: this pass binds a light index at CB_MATERIAL's slot
// and the G-buffer over the material textures. Pulling in the material set would collide on
// both. See Registers.hlsli.
#include "PBRCommon.hlsl"

POSITION_ONLY_PS_IN VSMain(VS_IN input)
{
    POSITION_ONLY_PS_IN output;

    float4 fragPos = mul(float4(input.pos, 1.0f), model);

    output.pos = mul(fragPos, viewProjection);
    output.fragPos = fragPos.xyz;
    output.depth = mul(fragPos, cameraView).z;

    return output;
}

cbuffer LightIndexBuffer : register(CB_LIGHT_INDEX)
{
    uint LightIndex;
}

Texture2D albedoBuffer : register(T_GBUFFER_ALBEDO);
Texture2D normalBuffer : register(T_GBUFFER_NORMAL);
Texture2D metallicRougnessAOBuffer : register(T_GBUFFER_MRAO);
Texture2D depthBuffer : register(T_GBUFFER_DEPTH);

float3 CalcLighting(float3 fragPos, float3 normal, float3 albedo, float metallic, float roughness);

[earlydepthstencil]
float4 PSMain(POSITION_ONLY_PS_IN input) : SV_Target0
{
    int2 uv = input.pos.xy;

    float depth = depthBuffer.Load(int3(uv, 0)).r;
    float3 albedo = albedoBuffer.Load(int3(uv, 0)).rgb;
    float3 normal = mul(normalBuffer.Load(int3(uv, 0)), cameraView).xyz;
    float3 metallicRougnessAO = metallicRougnessAOBuffer.Load(int3(uv, 0)).rgb;

    float metallic = metallicRougnessAO.x;
    float roughness = metallicRougnessAO.y;

    float3 fragPos = ScreenToView(float4(uv, depth, 1.0f)).xyz;

    float3 lit = CalcLighting(fragPos, normal, albedo, metallic, roughness);

    return float4(lit, 1.0f);
}

float3 CalcLighting(float3 fragPos, float3 normal, float3 albedo, float metallic, float roughness)
{
    //shading happens in view space, where the camera sits at the origin
    float3 viewDir = normalize(-fragPos);

    Light light = lights[LightIndex];

    float3 lit = 0.0f;
    if (light.type == POINT_LIGHT)
        lit = CalcPointLightInViewSpace(light, normal, fragPos, viewDir, albedo, metallic, roughness);
    else if (light.type == SPOT_LIGHT)
        lit = CalcSpotLightInViewSpace(light, normal, fragPos, viewDir, albedo, metallic, roughness);

    return lit;
}
