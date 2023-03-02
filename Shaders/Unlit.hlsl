cbuffer CameraConstantBuffer : register(b0)
{
	row_major matrix viewProjection;
};

struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output;

	float4 pos = float4(input.pos, 1.0f);

	output.pos = mul(pos, viewProjection);
	output.normal = input.normal;
	output.uv = input.uv;

	return output;
}

Texture2D my_texture;
SamplerState my_sampler;

float4 PSMain(PS_IN input) : SV_Target
{
	float normalR = (input.normal.r + 1.0f) / 2.0f;
	float normalG = (input.normal.g + 1.0f) / 2.0f;
	float normalB = (input.normal.b + 1.0f) / 2.0f;

	float4 col = float4(normalR, normalG, normalB, 1.0f);

	return my_texture.Sample(my_sampler, input.uv);
	//return col;
}