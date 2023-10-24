#include "../Quad.hlsl"
#include "PostProcessing.hlsl"

float4 PSMain(PS_IN input) : SV_Target
{
    return colorTexture.Sample(linearSampler, input.uv);
}
