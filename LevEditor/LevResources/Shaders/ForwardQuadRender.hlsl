
float4 VSMain(uint vI : SV_VERTEXID) : SV_POSITION
{
	float2 texcoord = float2(vI & 1, vI >> 1);
	return float4((texcoord.x - 0.5f) * 2, -(texcoord.y - 0.5f) * 2, 0, 1);
}

Texture2D colorTexture : register(t0);

float3 Uncharted2ToneMapping(float3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	float exposure = 2.;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	return color;
}

float3 ReinhardToneMapping(float3 color)
{
	float exposure = 1.5;
	color *= exposure/(1. + color / exposure);
	return color;
}

float4 PSMain(float4 pos : SV_POSITION) : SV_Target
{
	int2 uv = pos.xy;
	float3 finalColor = colorTexture.Load(int3(uv, 0)).rgb;
	
	//Gamma correction
    finalColor = pow(ReinhardToneMapping(finalColor), 0.45);

	return float4(finalColor, 1.0f);
}