cbuffer CameraConstantBuffer : register(b0)
{
	row_major matrix view;
	row_major matrix viewProjection;
	float3 cameraPosition;
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

	float4 pos = float4(input.pos * 1000, 0.0f);
	output.pos = mul(pos, viewProjection).xyww;
	output.uv = input.pos;

	//output.uv.x = -pos.x;
	//output.uv.yz = pos.yz;

	return output;
}

TextureCube tex;
SamplerState sam;

float4 PSMain(PS_IN input) : SV_Target
{
	return tex.Sample(sam, input.uv);
}