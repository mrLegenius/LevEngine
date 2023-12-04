#include "ShaderCommon.hlsl"
#include "MaterialPBR.hlsl"

PS_IN VSMain(VS_IN input)
{
	PS_IN output;

	row_major matrix boneTransform = CalculateBoneTransform(input.boneIds, input.boneWeights);
	
	float4 pos = mul(float4(input.pos, 1.0f), boneTransform);
	float4 fragPos = mul(pos, model);

	output.pos = mul(fragPos, viewProjection);
	output.uv = input.uv;
	output.fragPos = fragPos.xyz;
	output.depth = mul(fragPos, cameraView).z;

	float3 normal = mul(mul(float4(input.normal, 0.0f), boneTransform), transposedInvertedModel);
	float3 tangent = mul(mul(float4(input.tangent, 0.0f), boneTransform), transposedInvertedModel);
	float3 binormal = mul(mul(float4(input.binormal, 0.0f), boneTransform), transposedInvertedModel);
	
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
	float4 color = albedoMap.Sample(albedoMapSampler, textureUV);
	float3 emissive = emissiveMap.Sample(emissiveMapSampler, textureUV).rgb;

	float alpha = color.a;

	clip (alpha - 0.1f);

	//Gamma correction
	float3 albedo = pow(color.rgb, 2.2) * material.tint;

	float metallic = metallicMap.Sample(metallicMapSampler, textureUV) * material.metallic;
	float roughness = roughnessMap.Sample(roughnessMapSampler, textureUV) * material.roughness;
	float ao = ambientOcclusionMap.Sample(ambientOcclusionMapSampler, textureUV);
	
	float3 lit = CalcLighting(input.fragPos, normal, input.depth, albedo, metallic, roughness);

	float3 ambientColor = globalAmbient * albedo * ao;

	float3 finalColor = ambientColor + lit + emissive;

	//Gamma correction
	finalColor = finalColor / (finalColor + 1.0);
	finalColor = pow(finalColor, 0.45);

	return float4(finalColor, alpha);
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