#include "../Quad.hlsl"
#include "PostProcessing.hlsl"

float4 PSMain(PS_IN input) : SV_Target
{
    return Blur(input.uv, float2(1, 0), constants.BloomBlurSigma);
}