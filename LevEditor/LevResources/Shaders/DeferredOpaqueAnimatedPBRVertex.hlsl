#include "ShaderCommonAnimated.hlsl"

PS_IN VSMain(VS_IN input)
{
	PS_IN output;

	row_major matrix boneTransform = CalculateBoneTransform(input.boneIds, input.boneWeights);
	
	float4 pos = mul(float4(input.pos, 1.0), boneTransform);
	float4 fragPos = mul(pos, model);

	output.pos = mul(fragPos, viewProjection);
	output.uv = input.uv;
	output.fragPos = fragPos.xyz;
	output.depth = mul(fragPos, cameraView).z;
	
	float3 normal = mul(mul(float4(input.normal, 0.0), boneTransform), transposedInvertedModel);
	float3 tangent = mul(mul(float4(input.tangent, 0.0), boneTransform), transposedInvertedModel);
	float3 binormal = mul(mul(float4(input.binormal, 0.0), boneTransform), transposedInvertedModel);
	
    output.TBN = float3x3(normalize(tangent),
                          normalize(binormal),
                          normalize(normal));
	
	return output;
}