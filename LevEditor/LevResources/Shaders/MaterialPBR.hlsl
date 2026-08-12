#ifndef LEV_MATERIAL_PBR_HLSL
#define LEV_MATERIAL_PBR_HLSL

// The PBR material binding set. Only geometry passes want this -- see PBRCommon.hlsl for the
// BRDF math on its own.

#include "PBRCommon.hlsl"

struct PBRMaterial
{
    float3 tint;

    float2 tiling;
    float2 offset;

    float metallic;
    float roughness;
};

cbuffer MaterialConstantBuffer : register(CB_MATERIAL)
{
    PBRMaterial material;
};

Texture2D albedoMap : register(T_PBR_ALBEDO);
SamplerState albedoMapSampler : register(S_PBR_ALBEDO);

Texture2D<float> metallicMap : register(T_PBR_METALLIC);
SamplerState metallicMapSampler : register(S_PBR_METALLIC);

Texture2D<float> roughnessMap : register(T_PBR_ROUGHNESS);
SamplerState roughnessMapSampler : register(S_PBR_ROUGHNESS);

Texture2D normalMap : register(T_PBR_NORMAL);
SamplerState normalMapSampler : register(S_PBR_NORMAL);

Texture2D<float> ambientOcclusionMap : register(T_PBR_AO);
SamplerState ambientOcclusionMapSampler : register(S_PBR_AO);

Texture2D emissiveMap : register(T_PBR_EMISSIVE);
SamplerState emissiveMapSampler : register(S_PBR_EMISSIVE);

#endif
