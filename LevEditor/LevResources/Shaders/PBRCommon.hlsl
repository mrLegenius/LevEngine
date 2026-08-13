#ifndef LEV_PBR_COMMON_HLSL
#define LEV_PBR_COMMON_HLSL

// BRDF and IBL math, with no dependency on the material constant buffer or its texture set.
// The deferred lighting pass includes this instead of MaterialPBR.hlsl so that CB_MATERIAL
// and t1..t4 stay free for CB_LIGHT_INDEX and the G-buffer. See Registers.hlsli.

#include "ShaderCommon.hlsl"
#include "Lighting.hlsl"

static const float pi = 3.14159265359;

TextureCube irradianceMap : register(T_IRRADIANCE);
SamplerState irradianceMapSampler : register(S_IRRADIANCE);

TextureCube prefilterMap : register(T_PREFILTER);
SamplerState prefilterMapSampler : register(S_PREFILTER);

Texture2D brdfLUT : register(T_BRDF_LUT);
SamplerState brdfLUTSampler : register(S_BRDF_LUT);

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
    // EnvironmentPrecomputePass::CreatePrefilterCubemap writes maxMipLevels = 7 mips, so the
    // highest addressable level is 6.
    const float MAX_REFLECTION_LOD = 6.0;
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
    float3 albedo, float metallic, float roughness,
    bool castsShadow)
{
    const float3 lightDir = normalize(-light.direction);

    float3 Lo = CalcPBR(lightDir, normal, viewDir, light.color, albedo, metallic, roughness);

    // The cascade shadow map is rendered for the first directional light only, so the others are
    // shaded unshadowed rather than reusing a shadow map that was not built for them.
    float shadow = castsShadow ? CalcShadow(fragPosLightSpace, normal, lightDir, cascade) : 1.0f;

    return Lo * shadow;
}

// Sum of every directional light in the scene.
float3 CalcDirLights(
    float3 normal, float3 viewDir,
    float4 fragPosLightSpace, float cascade,
    float3 albedo, float metallic, float roughness)
{
    float3 result = 0.0f;

    for (int i = 0; i < dirLightsCount; i++)
    {
        result += CalcDirLight(dirLights[i], normal, viewDir, fragPosLightSpace, cascade,
                               albedo, metallic, roughness, i == 0);
    }

    return result;
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
    float3 lightDir = light.positionViewSpace.xyz - fragPos;
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
    float3 lightDir = light.positionViewSpace.xyz - fragPos;
    float distance = length(lightDir);
    lightDir = lightDir / distance;

    float attenuation = CalcAttenuation(light.range, light.smoothness, distance);
    float spotIntensity = CalcSpotConeInViewSpace(light, lightDir);
    float3 color = light.color * attenuation * spotIntensity * light.intensity;

    float3 Lo = CalcPBR(lightDir, normal, viewDir, color, albedo, metallic, roughness);

    return Lo;
}

#endif
