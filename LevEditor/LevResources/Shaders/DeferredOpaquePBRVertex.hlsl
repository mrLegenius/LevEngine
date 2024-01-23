#include "ShaderCommon.hlsl"

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