float4 VSMain(uint vI : SV_VERTEXID) : SV_POSITION
{
	float2 texcoord = float2(vI & 1, vI >> 1);
	return float4((texcoord.x - 0.5f) * 2, -(texcoord.y - 0.5f) * 2, 0, 1);
}

Texture2D colorTexture : register(t0);

float4 PSMain(float4 pos : SV_POSITION) : SV_Target
{
	int2 uv = pos.xy;
	float3 color = colorTexture.Load(int3(uv, 0)).rgb;

	return float4(color, 1.0f);
}