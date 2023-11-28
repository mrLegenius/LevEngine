cbuffer CameraConstantBuffer : register(b6)
{
	row_major matrix viewProjection;
};

struct VS_IN
{
	float3 pos : POSITION;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float3 uv : UV;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output;

	float4 pos = float4(input.pos, 0.0f);
	output.pos = mul(pos, viewProjection).xyww;
    output.uv = input.pos;

	return output;
}

TextureCube Cubemap;
SamplerState Sampler;

float4 PSMain(PS_IN input) : SV_Target
{
	float3 finalColor = Cubemap.Sample(Sampler, input.uv).rgb;
	return float4(finalColor, 1.0f);
}