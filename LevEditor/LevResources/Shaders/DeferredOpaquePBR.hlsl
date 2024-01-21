#include "ShaderCommon.hlsl"
#include "MaterialPBR.hlsl"

PS_IN VSMain(VS_IN input)
{
	PS_IN output;

	float4 pos = float4(input.pos, 1.0);
	float4 fragPos = mul(pos, model);

	output.pos = mul(fragPos, viewProjection);
	output.uv = input.uv;
	output.fragPos = fragPos.xyz;
	output.depth = mul(fragPos, cameraView).z;
	
	float3 normal = mul(float4(input.normal, 0.0), transposedInvertedModel);
	float3 tangent = mul(float4(input.tangent, 0.0), transposedInvertedModel);
	float3 binormal = mul(float4(input.binormal, 0.0), transposedInvertedModel);
	
    output.TBN = float3x3(normalize(tangent),
                          normalize(binormal),
                          normalize(normal));
	
	return output;
}

struct PS_OUT
{
	float4 LightAccumulation : SV_Target0;
	float4 Albedo : SV_Target1;
	float4 Normal : SV_Target2;
	float4 MetallicRoughnessAO : SV_Target3;
};

[earlydepthstencil]
PS_OUT PSMain(PS_IN input)
{
	PS_OUT result;

    float2 textureUV = ApplyTextureProperties(input.uv, material.tiling, material.offset);
	float3 normal = CalculateNormal(normalMap, normalMapSampler, textureUV, input.TBN);

	float3 emissive = emissiveMap.Sample(emissiveMapSampler, textureUV).rgb;
    float3 albedo = albedoMap.Sample(albedoMapSampler, textureUV).rgb * material.tint;
	float metallic = metallicMap.Sample(metallicMapSampler, textureUV) * material.metallic;
	float roughness = roughnessMap.Sample(roughnessMapSampler, textureUV) * material.roughness;
	float ao = ambientOcclusionMap.Sample(ambientOcclusionMapSampler, textureUV);

	float cascade = GetCascadeIndex(input.depth);
	float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);
	float3 viewDir = normalize(cameraPosition - input.fragPos);

	float3 ambient = CalcAmbient(normal, viewDir, albedo, metallic, roughness, ao);
	float3 lit = CalcDirLight(dirLight, normal, viewDir, fragPosLightSpace, cascade, albedo, metallic, roughness);

	float3 finalColor = ambient + lit + emissive;

	result.LightAccumulation = float4(finalColor, 1.0f);
    result.Albedo = float4(albedo, 1.0f);
    result.MetallicRoughnessAO = float4(metallic, roughness, ao, 0.0f);
    result.Normal = float4(normal, 0.0f);

	return result;
}