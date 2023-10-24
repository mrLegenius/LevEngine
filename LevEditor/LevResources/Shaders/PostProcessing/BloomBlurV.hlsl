#include "../Quad.hlsl"
#include "PostProcessing.hlsl"

float4 PSMain(PS_IN input) : SV_Target
{
    return Blur(input.uv, float2(0, 1), constants.BloomBlurSigma);
}