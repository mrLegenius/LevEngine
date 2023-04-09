#include "ShaderCommon.hlsl"

PS_IN VSMain(VS_IN input)
{
	PS_IN output;

	float4 pos = float4(input.pos, 1.0f);
	float4 fragPos = mul(pos, model);

	output.pos = mul(fragPos, viewProjection);
	output.normal = mul(transpose(model), input.normal);
	output.uv = input.uv;
	output.fragPos = fragPos.xyz;
	output.depth = mul(fragPos, cameraView).z;

	return output;
}

Texture2D diffuseTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D normalTexture : register(t3);
Texture2D depthTexture : register(t4);

LightingResult CalcLighting(float3 fragPos, float3 normal, float depth, float shininess);

[earlydepthstencil]
float4 PSMain(PS_IN input) : SV_Target
{
	int2 uv = input.pos.xy;
	float depth = depthTexture.Load(int3(uv, 0)).r;
	float4 fragPos = ScreenToView(float4(uv, depth, 1.0f));
	float3 normal = normalTexture.Load(int3(uv, 0)).rgb;

	float4 specularValues = specularTexture.Load(int3(uv, 0));
	float shininess = exp2(specularValues.a * 10.5f);

	LightingResult lit = CalcLighting(fragPos, normal, depth, shininess);

	float3 diffuse = diffuseTexture.Load(int3(uv, 0)).rgb;
	float3 specular = specularValues.rgb;

	float4 cameraPosition = { 0, 0, 0, 1 };
	float3 viewDir = normalize(cameraPosition - fragPos);

	LightingResult result;
	PointLight light = pointLights[pointLightsCount];
	float3 lightDir = light.positionViewSpace - fragPos;
	float distance = length(lightDir);
	lightDir = lightDir / distance;

	float attenuation = CalcAttenuation(light.range, light.smoothness, distance);

	result.diffuse = CalcDiffuse(light.color, lightDir, normal) * attenuation * light.intensity;
	result.specular = CalcSpecular(light.color, viewDir, lightDir, normal, shininess) * attenuation * light.intensity;

	float3 finalColor = (diffuse * lit.diffuse + specular * lit.specular);

	//return float4(result.diffuse, 1.0f);
    return float4(finalColor, 1.0f);
}

LightingResult CalcLighting(float3 fragPos, float3 normal, float depth, float shininess)
{
	float4 cameraPosition = { 0, 0, 0, 1 };
	float3 viewDir = normalize(cameraPosition - fragPos);
	
	return CalcPointLight(pointLights[pointLightsCount], normal, fragPos, viewDir, shininess);
}