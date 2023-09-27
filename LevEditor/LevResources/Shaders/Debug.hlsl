#include "ShaderCommon.hlsl"

struct DEBUG_VS_IN
{
    float3 pos : POSITION;
};

struct DEBUG_PS_IN
{
    float4 pos : SV_POSITION;
};

cbuffer DebugColor : register(b2)
{
	float4 color;
};

DEBUG_PS_IN VSMain(DEBUG_VS_IN input)
{
    DEBUG_PS_IN output;

    float4 pos = float4(input.pos, 1.0f);
    float4 fragPos = mul(pos, model);
    output.pos = mul(fragPos, viewProjection);
    
    return output;
}

float4 PSMain(DEBUG_PS_IN input) : SV_Target
{
    return color;
}
