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
	
	float3 normal = mul(float4(input.normal, 0.0f), transposedInvertedModel);
	float3 tangent = mul(float4(input.tangent, 0.0f), transposedInvertedModel);
	float3 binormal = mul(float4(input.binormal, 0.0f), transposedInvertedModel);
	
    output.TBN = float3x3(normalize(tangent),
                          normalize(binormal),
                          normalize(normal));
	
	return output;
}

float3 CalcLighting(float3 fragPos, float3 normal, float depth, float3 albedo, float metallic, float roughness);

[earlydepthstencil]
float4 PSMain(PS_IN input) : SV_Target
{
    float2 textureUV = ApplyTextureProperties(input.uv, material.tiling, material.offset);
		
	float3 normal = CalculateNormal(normalMap, normalMapSampler, textureUV, input.TBN);

	float3 albedo = albedoMap.Sample(albedoMapSampler, textureUV).rgb * material.tint;

	//Gamma correction
	albedo = pow(albedo, float3(2.2, 2.2, 2.2));

	float metallic = metallicMap.Sample(metallicMapSampler, textureUV) * material.metallic;
	float roughness = roughnessMap.Sample(roughnessMapSampler, textureUV) * material.roughness;
	float ao = ambientOcclusionMap.Sample(ambientOcclusionMapSampler, textureUV);
	
	float3 lit = CalcLighting(input.fragPos, normal, input.depth, albedo, metallic, roughness);

	float ambientColor = globalAmbient * albedo * ao;

	float3 finalColor = ambientColor + lit;

	//Gamma correction
	finalColor = finalColor / (finalColor + 1.0);
	finalColor = pow(finalColor, 0.44);

	return float4(finalColor, 1.0);
}

float3 CalcLighting(float3 fragPos, float3 normal, float depth, float3 albedo, float metallic, float roughness)
{
	float cascade = GetCascadeIndex(depth);
	float4 fragPosLightSpace = mul(float4(fragPos, 1.0f), lightViewProjection[cascade]);

	float3 viewDir = normalize(cameraPosition - fragPos);

	float3 totalResult = CalcDirLight(dirLight, normal, viewDir, fragPosLightSpace, cascade, albedo, metallic, roughness);

	for (int i = 0; i < pointLightsCount; i++)
		totalResult += CalcPointLight(pointLights[i], normal, fragPos, viewDir, albedo, metallic, roughness);

	totalResult = saturate(totalResult);

	return totalResult;
}