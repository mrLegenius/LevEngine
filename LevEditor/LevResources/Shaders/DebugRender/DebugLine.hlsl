#include "../ShaderCommon.hlsl"

struct DEBUG_VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct DEBUG_PS_IN
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

DEBUG_PS_IN VSMain(DEBUG_VS_IN input)
{
    DEBUG_PS_IN output;

    float4 pos = float4(input.pos, 1.0f);
    output.pos = mul(pos, viewProjection);
    output.color = input.color;
    
    return output;
}

float4 PSMain(DEBUG_PS_IN input) : SV_Target
{
    return input.color;
}
