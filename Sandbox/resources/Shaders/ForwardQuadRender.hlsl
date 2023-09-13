#include "ShaderCommon.hlsl"

float4 VSMain(uint vI : SV_VERTEXID) : SV_POSITION
{
	float2 texcoord = float2(vI & 1, vI >> 1);
	return float4((texcoord.x - 0.5f) * 2, -(texcoord.y - 0.5f) * 2, 0, 1);
}

Texture2D diffuseTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D normalTexture : register(t3);
Texture2D depthTexture : register(t4);

Texture2DArray shadowMapTexture : register(t9);

LightingResult CalcLighting(float3 fragPos, float3 normal, float depth, float shininess);

[earlydepthstencil]
float4 PSMain(float4 pos : SV_POSITION) : SV_Target
{
	int2 uv = pos.xy;
	float depth = depthTexture.Load(int3(uv, 0)).r;
	float4 fragPos = ScreenToView(float4(uv, depth, 1.0f));
	float3 normal = normalTexture.Load(int3(uv, 0)).rgb;

	float4 specularValues = specularTexture.Load(int3(uv, 0));
	float shininess = exp2(specularValues.a * 10.5f);
	
	LightingResult lit = CalcLighting(fragPos, normal, depth, shininess);
	float3 diffuse = diffuseTexture.Load(int3(uv, 0)).rgb * lit.diffuse;
	float3 specular = specularValues.rgb * lit.specular;
	
	float3 finalColor = (diffuse + specular);

	return float4(finalColor, 1.0f);
	//return float4(depth, depth, depth, 1.0f);
}

LightingResult CalcLighting(float3 fragPos, float3 normal, float depth, float shininess)
{
	float4 cameraPosition = { 0, 0, 0, 1 };

	float cascade = 0;// GetCascadeIndex(depth);
	float4 fragPosLightSpace = float4(fragPos, 1.0f);// mul(float4(fragPos, 1.0f), lightViewProjection[cascade]);

	float3 viewDir = normalize(cameraPosition - fragPos);

	LightingResult totalResult = CalcDirLight(dirLight, normal, viewDir, fragPosLightSpace, cascade, shininess);

	/*[unroll]
	for (int i = 0; i < pointLightsCount; i++)
	{
		LightingResult result = CalcPointLight(pointLights[i], normal, fragPos, viewDir);
		totalResult.diffuse += result.diffuse;
		totalResult.specular += result.specular;
	}*/

	totalResult.diffuse = saturate(totalResult.diffuse);
	totalResult.specular = saturate(totalResult.specular);

	return totalResult;
}