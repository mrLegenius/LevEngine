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

LightingResult CalcLighting(float3 fragPos, float3 normal, float depth);

[earlydepthstencil]
float4 PSMain(PS_IN input) : SV_Target
{
    float2 textureUV = ApplyTextureProperties(input.uv, material.tiling, material.offset);
		
	float3 normal = CalculateNormal(normalTexture, normalTextureSampler, textureUV, input.TBN);

	LightingResult lit = CalcLighting(input.fragPos, normal, input.depth);

    float3 emissive = CombineColorAndTexture(material.emissive, emissiveTexture, emissiveTextureSampler, textureUV);
    float3 ambient = CombineColorAndTexture(material.ambient, diffuseTexture, diffuseTextureSampler, textureUV) * globalAmbient;
    float3 diffuse = CombineColorAndTexture(material.diffuse, diffuseTexture, diffuseTextureSampler, textureUV) * lit.diffuse;

	float3 specular = 0;
	if (material.shininess > 1.0f)
        specular = CombineColorAndTexture(material.specular, specularTexture, specularTextureSampler, textureUV) * lit.specular;

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