#include "../Quad.hlsl"
#include "PostProcessing.hlsl"

float PSMain(PS_IN input) : SV_Target
{
    // Sample the input
    float3 color = colorTexture.Sample(linearSampler, input.uv).rgb;

    // calculate the luminance using a weighted average
    float luminance = max(dot(color, float3(0.299f, 0.587f, 0.114f)), 0.0001f);
    luminance = log(max(luminance, 0.00001f));

    return luminance;
}