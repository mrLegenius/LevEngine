#include "ShaderCommon.hlsl"
#include "MaterialPBR.hlsl"

PS_IN VSMain(VS_IN input)
{
	PS_IN output;

	VertexCalculationResult result = CalculateVertex(input);

	float4 fragPos = mul(result.pos, model);
	float3 binormal = cross(result.normal, result.tangent);

	output.pos = mul(fragPos, viewProjection);
	output.uv = input.uv;
	output.fragPos = fragPos.xyz;
	output.depth = mul(fragPos, cameraView).z;

    output.TBN = float3x3(normalize(result.tangent),
                          normalize(binormal),
                          normalize(result.normal));
	
	return output;
}

float3 CalcLighting(float3 fragPos, float3 normal, float depth, float3 albedo, float metallic, float roughness, float ao);

[earlydepthstencil]
float4 PSMain(PS_IN input) : SV_Target
{
    float2 textureUV = ApplyTextureProperties(input.uv, material.tiling, material.offset);
	float3 normal = CalculateNormal(normalMap, normalMapSampler, textureUV, input.TBN);
	float4 color = albedoMap.Sample(albedoMapSampler, textureUV);
	float3 emissive = emissiveMap.Sample(emissiveMapSampler, textureUV).rgb;

	float alpha = color.a;

	clip (alpha - 0.1f);

	float3 albedo = color.rgb * material.tint;

	float metallic = metallicMap.Sample(metallicMapSampler, textureUV) * material.metallic;
	float roughness = roughnessMap.Sample(roughnessMapSampler, textureUV) * material.roughness;
	float ao = ambientOcclusionMap.Sample(ambientOcclusionMapSampler, textureUV);
	
	float3 lit = CalcLighting(input.fragPos, normal, input.depth, albedo, metallic, roughness, ao);

	float3 finalColor = lit + emissive;

	//Gamma correction
	finalColor = finalColor / (finalColor + 1.0);
	finalColor = pow(finalColor, 0.45);

	return float4(finalColor, alpha);
}

float3 CalcLighting(float3 fragPos, float3 normal, float depth, float3 albedo, float metallic, float roughness, float ao)
{
	float cascade = GetCascadeIndex(depth);
	float4 fragPosLightSpace = mul(float4(fragPos, 1.0f), lightViewProjection[cascade]);

	float3 viewDir = normalize(cameraPosition - fragPos);

	float3 ambient = CalcAmbient(normal, viewDir, albedo, metallic, roughness, ao);

	float3 totalResult = CalcDirLight(dirLight, normal, viewDir, fragPosLightSpace, cascade, albedo, metallic, roughness);

	for (int i = 0; i < lightsCount; i++)
	{	
		Light light = lights[i];
		
		float3 lit = 0.0f;
		if (light.type == POINT_LIGHT)
			totalResult += CalcPointLight(light, normal, fragPos, viewDir, albedo, metallic, roughness);
		else if (light.type == SPOT_LIGHT)
			totalResult += CalcSpotLight(light, normal, fragPos, viewDir, albedo, metallic, roughness);
	}

	return totalResult + ambient;
}