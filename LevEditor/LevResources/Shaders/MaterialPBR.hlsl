#include "Lighting.hlsl"

static const float pi = 3.14159265359;

struct PBRMaterial
{
    float3 tint;
    
    float2 tiling;
    float2 offset;
    
    float metallic;
    float roughness;
};

cbuffer MaterialConstantBuffer : register(b4)
{
    PBRMaterial material;
};

Texture2D albedoMap : register(t0);
SamplerState albedoMapSampler : register(s0);

Texture2D<float> metallicMap : register(t1);
SamplerState metallicMapSampler : register(s1);

Texture2D<float> roughnessMap : register(t2);
SamplerState roughnessMapSampler : register(s2);

Texture2D normalMap : register(t3);
SamplerState normalMapSampler : register(s3);

Texture2D<float> ambientOcclusionMap : register(t4);
SamplerState ambientOcclusionMapSampler : register(s4);

Texture2D emissiveMap : register(t5);
SamplerState emissiveMapSampler : register(s5);

TextureCube irradianceMap : register(t10);
SamplerState irradianceMapSampler : register(s10);

TextureCube prefilterMap : register(t11);
SamplerState prefilterMapSampler : register(s11);

Texture2D brdfLUT : register(t12);
SamplerState brdfLUTSampler : register(s12);

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max((float3)(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = pi * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	
    return ggx1 * ggx2;
}

float3 CalcPBR(
    float3 lightDir,
    float3 normal, float3 viewDir, float3 color, 
    float3 albedo, float metallic, float roughness)
{
    float3 halfVector = normalize(viewDir + lightDir);
    
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);

    float NDF = DistributionGGX(normal, halfVector, roughness);
    float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    float3 F = FresnelSchlick(max(dot(halfVector, viewDir), 0.0), F0);
    
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    float3 specular = numerator / denominator;
    
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    float NdotL = max(dot(normal, lightDir), 0.0);
    float3 Lo = (kD * albedo / pi + specular) * color * NdotL;

    return Lo;
}

float3 CalcAmbient(
    float3 normal, float3 viewDir, 
    float3 albedo, float metallic, float roughness, float ao)
{
    float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);

    float3 F = FresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    float3 irradiance = irradianceMap.Sample(irradianceMapSampler, normal).rgb;
    float3 diffuse = irradiance * albedo;

    float3 R = reflect(-viewDir, normal);
    const float MAX_REFLECTION_LOD = 7.0;
    float3 prefilteredColor = prefilterMap.SampleLevel(prefilterMapSampler, R, roughness * MAX_REFLECTION_LOD).rgb; 
    float2 brdf  = brdfLUT.Sample(brdfLUTSampler, float2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    float3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    float3 ambient = (kD * diffuse + specular) * ao;

    return ambient;
}

float3 CalcDirLight(
    DirLight light,
    float3 normal, float3 viewDir,
    float4 fragPosLightSpace, float cascade,
    float3 albedo, float metallic, float roughness)
{
    const float3 lightDir = normalize(-light.direction);
    
    float3 Lo = CalcPBR(lightDir, normal, viewDir, light.color, albedo, metallic, roughness);
    float shadow = CalcShadow(fragPosLightSpace, normal, lightDir, cascade);

    return Lo * shadow;
}

float3 CalcPointLight(
    Light light,
    float3 normal, float3 fragPos, float3 viewDir,
    float3 albedo, float metallic, float roughness)
{
    float3 lightDir = light.position - fragPos;
    float distance = length(lightDir);
    lightDir = lightDir / distance;

    float attenuation = CalcAttenuation(light.range, light.smoothness, distance);
    float3 color = light.color * attenuation * light.intensity;
    
    float3 Lo = CalcPBR(lightDir, normal, viewDir, color, albedo, metallic, roughness);

    return Lo;
}

float3 CalcSpotLight(
    Light light,
    float3 normal, float3 fragPos, float3 viewDir,
    float3 albedo, float metallic, float roughness)
{
    float3 lightDir = light.position - fragPos.xyz;
    float distance = length(lightDir);
    lightDir = lightDir / distance;

    float attenuation = CalcAttenuation(light.range, light.smoothness, distance);
    float spotIntensity = CalcSpotCone(light, lightDir);
    float3 color = light.color * attenuation * spotIntensity * light.intensity;
    
    float3 Lo = CalcPBR(lightDir, normal, viewDir, color, albedo, metallic, roughness);

    return Lo;
}

float3 CalcPointLightInViewSpace(
    Light light, 
    float3 normal, float3 fragPos, float3 viewDir,
    float3 albedo, float metallic, float roughness)
{
    float3 lightDir = light.positionViewSpace - fragPos;
    float distance = length(lightDir);
    lightDir = lightDir / distance;

    float attenuation = CalcAttenuation(light.range, light.smoothness, distance);
    float3 color = light.color * attenuation * light.intensity;

    float3 Lo = CalcPBR(lightDir, normal, viewDir, color, albedo, metallic, roughness);

    return Lo;
}


float3 CalcSpotLightInViewSpace(
    Light light,
    float3 normal, float3 fragPos, float3 viewDir,
    float3 albedo, float metallic, float roughness)
{
    float3 lightDir = light.positionViewSpace - fragPos;
    float distance = length(lightDir);
    lightDir = lightDir / distance;

    float attenuation = CalcAttenuation(light.range, light.smoothness, distance);
    float spotIntensity = CalcSpotConeInViewSpace(light, lightDir);
    float3 color = light.color * attenuation * spotIntensity * light.intensity;
    
    float3 Lo = CalcPBR(lightDir.xyz, normal, viewDir, color, albedo, metallic, roughness);

    return Lo;
}


