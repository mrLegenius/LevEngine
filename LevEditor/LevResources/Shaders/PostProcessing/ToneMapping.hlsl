#include "../Quad.hlsl"
#include "PostProcessing.hlsl"

Texture2D<float> averageLuminanceMap : register(t1);
Texture2D bloomMap : register(t2);

float3 Uncharted2ToneMapping(float3 color, float exposure)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    //float exposure = 2.;
    color *= exposure;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    return color;
}

float3 ReinhardToneMapping(float3 color, float exposure)
{
    //float exposure = 1.5;
    return color * exposure / (1. + color / exposure);

    //return color / (1 + color);
}

float3 WhitePreservingLumaBasedReinhardToneMapping(float3 color)
{
    float white = 2.;
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma * (1. + luma / (white*white)) / (1. + luma);
    color *= toneMappedLuma / luma;
    return color;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float2 uv = input.uv;
    float3 color = colorTexture.Sample(linearSampler, uv).rgb;

    float averageLuminance = averageLuminanceMap.Load(int3(0, 0, 0));
    float exposure = CalculateExposure(averageLuminance);
	exposure = clamp(exposure, constants.MinExposure, constants.MaxExposure);

    // Sample the bloom
    float3 bloom = bloomMap.Sample(linearSampler, uv).rgb;
    bloom *= constants.BloomMagnitude;
    color += bloom;

    //Tone mapping
    //float3 finalColor = Uncharted2ToneMapping(color, exposure);
    float3 finalColor = ReinhardToneMapping(color, exposure);

    //Gamma correction
    finalColor = pow(finalColor, 0.45);

    // Sample the bloom
    // float3 bloom = bloomMap.Sample(linearSampler, uv).rgb;
    // bloom *= constants.BloomMagnitude;
    // finalColor += bloom;

    return float4(finalColor, 1.0f);
}