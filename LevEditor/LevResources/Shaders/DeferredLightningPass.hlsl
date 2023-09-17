#include "ShaderCommon.hlsl"

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

Texture2D diffuseMap : register(t1);
Texture2D specularMap : register(t2);
Texture2D normalMap : register(t3);
Texture2D depthMap : register(t4);

LightingResult CalcLighting(float3 fragPos, float3 normal, float depth, float shininess);

[earlydepthstencil]
float4 PSMain(PS_IN input) : SV_Target
{
	int2 uv = input.pos.xy;
    float depth = depthMap.Load(int3(uv, 0)).r;
	float4 fragPos = ScreenToView(float4(uv, depth, 1.0f));
    float3 normal = mul(normalMap.Load(int3(uv, 0)), cameraView).rgb;

    float4 specularValues = specularMap.Load(int3(uv, 0));
	float shininess = exp2(specularValues.a * 10.5f);

	LightingResult lit = CalcLighting(fragPos, normal, depth, shininess);

    float3 diffuse = diffuseMap.Load(int3(uv, 0)).rgb;
	float3 specular = specularValues.rgb;

	float3 finalColor = (diffuse * lit.diffuse + specular * lit.specular);

    return float4(finalColor, 1.0f);
}

LightingResult CalcLighting(float3 fragPos, float3 normal, float depth, float shininess)
{
	float4 cameraPosition = { 0, 0, 0, 1 };
	float3 viewDir = normalize(cameraPosition - fragPos);
	
    return CalcPointLightInViewSpace(pointLights[LightIndex], normal, fragPos, viewDir, shininess);
}