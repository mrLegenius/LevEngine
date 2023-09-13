cbuffer CameraConstantBuffer : register(b0)
{
	row_major matrix viewProjection;
	float3 cameraPosition;
};

cbuffer ModelConstantBuffer : register(b1)
{
	row_major matrix model;
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
	matrix mvp = mul(model, viewProjection);
	output.pos = mul(pos, mvp);
	output.normal = input.normal;
	output.uv = input.uv;

	return output;
}

Texture2D my_texture;
SamplerState my_sampler;

float4 PSMain(PS_IN input) : SV_Target
{
	return my_texture.Sample(my_sampler, input.uv);
}