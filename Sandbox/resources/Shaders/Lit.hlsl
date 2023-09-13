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

LightingResult CalcLighting(float3 fragPos, float3 normal, float depth);

[earlydepthstencil]
float4 PSMain(PS_IN input) : SV_Target
{
	float3 normal;
	
    float2 textureUV = ApplyTextureProperties(input.uv, material.tiling, material.offset);
		
	if (material.hasNormalTexture)
	{
        float3 texNormal = normalTexture.Sample(normalTextureSampler, textureUV).rgb;
		normal = normalize(texNormal * 2.0 - 1.0);
	}
	else
	{
		normal = normalize(input.normal);
	}

	LightingResult lit = CalcLighting(input.fragPos, normal, input.depth);

    float3 emissive = CombineColorAndTexture(material.emissive, emissiveTexture, emissiveTextureSampler, material.hasEmissiveTexture, textureUV);
    float3 ambient = CombineColorAndTexture(material.diffuse, diffuseTexture, diffuseTextureSampler, material.hasDiffuseTexture, textureUV) * globalAmbient;
    float3 diffuse = CombineColorAndTexture(material.diffuse, diffuseTexture, diffuseTextureSampler, material.hasDiffuseTexture, textureUV) * lit.diffuse;

	float3 specular = 0;
	if (material.shininess > 1.0f)
        specular = CombineColorAndTexture(material.specular, specularTexture, specularTextureSampler, material.hasSpecularTexture, textureUV) * lit.specular;

	float3 finalColor = (emissive + ambient + diffuse + specular);

	return float4(finalColor, 1.0);
}

LightingResult CalcLighting(float3 fragPos, float3 normal, float depth)
{
	float cascade = GetCascadeIndex(depth);
	float4 fragPosLightSpace = mul(float4(fragPos, 1.0f), lightViewProjection[cascade]);

	float3 viewDir = normalize(cameraPosition - fragPos);

	LightingResult totalResult = CalcDirLight(dirLight, normal, viewDir, fragPosLightSpace, cascade, material.shininess);

	for (int i = 0; i < pointLightsCount; i++)
	{
        LightingResult result = CalcPointLight(pointLights[i], normal, fragPos, viewDir, material.shininess);
		totalResult.diffuse += result.diffuse;
		totalResult.specular += result.specular;
	}

	totalResult.diffuse = saturate(totalResult.diffuse);
	totalResult.specular = saturate(totalResult.specular);

	return totalResult;
}