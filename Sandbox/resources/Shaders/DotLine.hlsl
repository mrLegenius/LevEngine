struct VS_IN
{
	float4 pos : POSITION0;
	float4 col : COLOR0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float4 col : COLOR;
};

PS_IN VSMain( VS_IN input )
{
	PS_IN output = (PS_IN)0;
	
	output.pos = input.pos;
	output.col = input.col;
	
	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 col = input.col;

#ifdef TEST
	if (input.pos.x > 400) col = TCOLOR;
#endif

	float dist = length((input.pos.xy - float2(0, -100000)));
	float u_dashSize = 10;
	float u_gapSize = 5;
	if (frac(dist / (u_dashSize + u_gapSize)) > u_dashSize / (u_dashSize + u_gapSize))
		discard;

	return col;
}