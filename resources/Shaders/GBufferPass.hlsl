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
	float3 normal;

	if (material.hasNormalTexture)
	{
        float3 texNormal = normalTexture.Sample(normalTextureSampler, ApplyTextureProperties(input.uv, material.normalProperties)).rgb;
		normal = normalize(texNormal * 2.0 - 1.0);
	}
	else
	{
		normal = normalize(input.normal);
	}

	result.Normal = float4(normal, 0.0f);

	float3 emissive = CombineColorAndTexture(material.emissive, emissiveTexture, emissiveTextureSampler, material.hasEmissiveTexture, input.uv, material.emissiveProperties);
    float3 ambient = CombineColorAndTexture(material.ambient, ambientTexture, ambientTextureSampler, material.hasAmbientTexture, input.uv, material.ambientProperties) * globalAmbient;

    float3 diffuse = CombineColorAndTexture(material.diffuse, diffuseTexture, diffuseTextureSampler, material.hasDiffuseTexture, input.uv, material.diffuseProperties);
	
	float cascade = GetCascadeIndex(input.depth);
	float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);

	float3 viewDir = normalize(cameraPosition - input.fragPos);

	LightingResult lit = CalcDirLight(dirLight, normal, viewDir, fragPosLightSpace, cascade, material.shininess);
	result.Diffuse = float4(diffuse, 1.0f);
	
	float3 specular = 0;
	if (material.shininess > 1.0f)
        specular = CombineColorAndTexture(material.specular, specularTexture, specularTextureSampler, material.hasSpecularTexture, input.uv, material.specularProperties);

	result.LightAccumulation = float4(ambient + emissive + (diffuse * lit.diffuse) + (specular * lit.specular), 1.0f);

	result.Specular = float4(specular, log2(material.shininess) / 10.5f);

	return result;
}