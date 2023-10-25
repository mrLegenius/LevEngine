#include "../Quad.hlsl"
#include "PostProcessing.hlsl"

Texture2D<float> luminanceMap : register(t1);

float4 PSMain(PS_IN input) : SV_Target
{
    float3 color = colorTexture.Sample(linearSampler, input.uv).rgb;

    // Tone map it to threshold
    // float avgLuminance = luminanceMap.Load(uint3(0, 0, 0));
    // float pixelLuminance = CalcLuminance(color);
    // color = color * CalculateExposure(avgLuminance, constants.BloomThreshold);
    //
    // if(dot(color, 0.333f) <= 0.001f)
    //     color = 0.0f;

    float pixelLuminance = CalcLuminance(color);
    if (pixelLuminance < constants.BloomThreshold)
        color = 0.0f;

    return float4(color, 1.0f);
}