#include "ShaderCommon.hlsl"
#include "MaterialSimple.hlsl"

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

struct PS_OUT
{
	float4 LightAccumulation : SV_Target0;
	float4 Diffuse : SV_Target1;
	float4 Specular : SV_Target2;
	float4 Normal : SV_Target3;
};

[earlydepthstencil]
PS_OUT PSMain(PS_IN input)
{
	PS_OUT result;

    float2 textureUV = ApplyTextureProperties(input.uv, material.tiling, material.offset);
	
	float3 normal = CalculateNormal(normalTexture, normalTextureSampler, textureUV, input.TBN);

	result.Normal = float4(normal, 0.0f);

    float3 emissive = CombineColorAndTexture(material.emissive, emissiveTexture, emissiveTextureSampler, textureUV);
    float3 ambient = CombineColorAndTexture(material.diffuse, diffuseTexture, diffuseTextureSampler, textureUV) * globalAmbient;

    float3 diffuse = CombineColorAndTexture(material.diffuse, diffuseTexture, diffuseTextureSampler, textureUV);
	
	float cascade = GetCascadeIndex(input.depth);
	float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);

	float3 viewDir = normalize(cameraPosition - input.fragPos);

	LightingResult lit = CalcDirLight(dirLight, normal, viewDir, fragPosLightSpace, cascade, material.shininess);
	result.Diffuse = float4(diffuse, 1.0f);
	
	float3 specular = 0;
	if (material.shininess > 1.0f)
        specular = CombineColorAndTexture(material.specular, specularTexture, specularTextureSampler, textureUV);

	result.LightAccumulation = float4(ambient + emissive + (diffuse * lit.diffuse) + (specular * lit.specular), 1.0f);

	result.Specular = float4(specular, log2(material.shininess) / 10.5f);

	return result;
}