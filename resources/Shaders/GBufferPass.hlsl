#include "ShaderCommon.hlsl"

PS_IN VSMain(VS_IN input)
{
	PS_IN output;

	float4 pos = float4(input.pos, 1.0f);
	float4 fragPos = mul(pos, model);

	output.pos = mul(fragPos, viewProjection);
	output.normal = mul(mul(transpose(model), input.normal), cameraView);
	output.uv = input.uv;
	output.fragPos = fragPos.xyz;
	output.depth = mul(fragPos, cameraView).z;

	return output;
}

Texture2D emissiveTexture : register(t0);
SamplerState emissiveTextureSampler : register(s0);

Texture2D ambientTexture : register(t1);
SamplerState ambientTextureSampler : register(s1);

Texture2D diffuseTexture : register(t2);
SamplerState diffuseTextureSampler : register(s2);

Texture2D specularTexture : register(t3);
SamplerState specularTextureSampler : register(s3);

Texture2D normalTexture : register(t4);
SamplerState normalTextureSampler : register(s4);

float3 CombineColorAndTexture(float3 color, Texture2D tex, SamplerState sampl, bool hasTexture, float2 uv);

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
		float3 texNormal = normalTexture.Sample(normalTextureSampler, input.uv).rgb;
		normal = normalize(texNormal * 2.0 - 1.0);
	}
	else
	{
		normal = normalize(input.normal);
	}

	result.Normal = float4(normal, 0.0f);

	float3 emissive = CombineColorAndTexture(material.emissive, emissiveTexture, emissiveTextureSampler, material.hasEmissiveTexture, input.uv);
	float3 ambient = CombineColorAndTexture(material.ambient, ambientTexture, ambientTextureSampler, material.hasAmbientTexture, input.uv) * globalAmbient;
	float3 diffuse = CombineColorAndTexture(material.diffuse, diffuseTexture, diffuseTextureSampler, material.hasDiffuseTexture, input.uv);
	
	float cascade = GetCascadeIndex(input.depth);
	float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);

	float3 viewDir = normalize(cameraPosition - input.fragPos);

	LightingResult lit = CalcDirLight(dirLight, normal, viewDir, fragPosLightSpace, cascade, material.shininess);
	result.Diffuse = float4(diffuse, 1.0f);
	
	float3 specular = 0;
	if (material.shininess > 1.0f)
		specular = CombineColorAndTexture(material.specular, specularTexture, specularTextureSampler, material.hasSpecularTexture, input.uv);

	result.LightAccumulation = float4(ambient + emissive + (diffuse * lit.diffuse) + (specular * lit.specular), 1.0f);

	result.Specular = float4(specular, log2(material.shininess) / 10.5f);

	return result;
}
float3 CombineColorAndTexture(float3 color, Texture2D tex, SamplerState sampl, bool hasTexture, float2 uv)
{
	float3 result = color;
	if (hasTexture)
	{
		float3 texColor = tex.Sample(sampl, uv);
		if (any(result.rgb))
		{
			result *= texColor;
		}
		else
		{
			result = texColor;
		}
	}

	return result;
}