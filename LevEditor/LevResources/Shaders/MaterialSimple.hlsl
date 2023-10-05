#include "Lightning.hlsl"

struct Material
{
    float3 ambient; //<--- 16 ---<<
    float3 emissive; //<--- 16 ---<<
    float3 diffuse; //<--- 16 ---<<
    float3 specular; //<--- 16 ---<<
	
	//<--- 16 ---<<
    float2 tiling;
    float2 offset;
	//<--- 16 ---<<

	//<--- 4 ---<<
    float shininess;
	//<--- 4 ---<<
};

cbuffer MaterialConstantBuffer : register(b4)
{
	Material material;
};

struct LightingResult
{
	float3 diffuse;
	float3 specular;
};

Texture2D emissiveTexture : register(t1);
SamplerState emissiveTextureSampler : register(s1);

Texture2D diffuseTexture : register(t2);
SamplerState diffuseTextureSampler : register(s2);

Texture2D specularTexture : register(t3);
SamplerState specularTextureSampler : register(s3);

Texture2D normalTexture : register(t4);
SamplerState normalTextureSampler : register(s4);

float3 CalcDiffuse(float3 color, float3 lightDir, float3 normal)
{
	float NdotL = max(0.0, dot(normal, lightDir));
	return color * NdotL;
}

float3 CalcSpecular(float3 color, float3 viewDir, float3 lightDir, float3 normal, float shininess)
{
	//Blinn-Phong
	float3 halfAngle = normalize(lightDir + viewDir);
	float spec = dot(normal, halfAngle);

	//Phong
	//float3 reflectDir = reflect(-lightDir, normal);
	//float spec = dot(viewDir, reflectDir);

	return color * pow(max(spec, 0.0), shininess);
}

LightingResult CalcDirLight(DirLight light, float3 normal, float3 viewDir, float4 fragPosLightSpace, float cascade, float shininess)
{
	LightingResult result;

	float3 lightDir = normalize(-light.direction);

	float shadow = CalcShadow(fragPosLightSpace, normal, lightDir, cascade);
	result.diffuse = CalcDiffuse(light.color, lightDir, normal) * shadow;
	result.specular = CalcSpecular(light.color, viewDir, lightDir, normal, shininess) * shadow;

	return result;
}

LightingResult CalcPointLight(PointLight light, float3 normal, float3 fragPos, float3 viewDir, float shininess)
{
	LightingResult result;

	float3 lightDir = light.position - fragPos;
	float distance = length(lightDir);
	lightDir = lightDir / distance;

	float attenuation = CalcAttenuation(light.range, light.smoothness, distance);

	result.diffuse = CalcDiffuse(light.color, lightDir, normal) * attenuation * light.intensity;
	result.specular = CalcSpecular(light.color, viewDir, lightDir, normal, shininess) * attenuation * light.intensity;

	return result;
}

LightingResult CalcPointLightInViewSpace(PointLight light, float3 normal, float3 fragPos, float3 viewDir, float shininess)
{
    LightingResult result;

    float3 lightDir = light.positionViewSpace - fragPos;
    float distance = length(lightDir);
    lightDir = lightDir / distance;

    float attenuation = CalcAttenuation(light.range, light.smoothness, distance);

    result.diffuse = CalcDiffuse(light.color, lightDir, normal) * attenuation * light.intensity;
    result.specular = CalcSpecular(light.color, viewDir, lightDir, normal, shininess) * attenuation * light.intensity;

    return result;
}