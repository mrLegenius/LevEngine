#include "ShaderCommon.hlsl"
#include "MaterialPBR.hlsl"

PS_IN VSMain(VS_IN input)
{
    PS_IN output;

    float4 pos = float4(input.pos, 1.0f);
    float4 fragPos = mul(pos, model);

    output.pos = mul(fragPos, viewProjection);
    output.uv = input.uv;
    output.fragPos = fragPos.xyz;
    output.depth = mul(fragPos, cameraView).z;

    return output;
}

cbuffer LightIndexBuffer : register(b4)
{
    uint LightIndex;
}

Texture2D albedoBuffer : register(t1);
Texture2D normalBuffer : register(t2);
Texture2D metallicRougnessAOBuffer : register(t3);
Texture2D depthBuffer : register(t4);

float3 CalcLighting(float3 fragPos, float3 normal, float depth, float3 albedo, float metallic, float roughness);

[earlydepthstencil]
float4 PSMain(PS_IN input) : SV_Target0
{
    int2 uv = input.pos.xy;

    float depth = depthBuffer.Load(int3(uv, 0)).r;
    float3 albedo = albedoBuffer.Load(int3(uv, 0)).rgb;
    float3 normal = mul(normalBuffer.Load(int3(uv, 0)), cameraView).rgb;
    float3 metallicRougnessAO = metallicRougnessAOBuffer.Load(int3(uv, 0)).rgb;

    float metallic = metallicRougnessAO.x;
    float roughness = metallicRougnessAO.y;

    float4 fragPos = ScreenToView(float4(uv, depth, 1.0f));

    float3 lit = CalcLighting(fragPos, normal, depth, albedo, metallic, roughness);

    return float4(lit, 1.0f);
}

float3 CalcLighting(float3 fragPos, float3 normal, float depth, float3 albedo, float metallic, float roughness)
{
    float4 cameraPos = { 0, 0, 0, 1 };
    float3 viewDir = normalize(cameraPos - fragPos);

    return CalcPointLightInViewSpace(pointLights[LightIndex], normal, fragPos, viewDir, albedo, metallic, roughness);
}