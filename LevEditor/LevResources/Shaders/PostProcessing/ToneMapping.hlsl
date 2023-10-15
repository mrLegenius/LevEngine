#include "PostProcessing.hlsl"

float4 VSMain(uint vI : SV_VERTEXID) : SV_POSITION
{
    float2 texcoord = float2(vI & 1, vI >> 1);
    return float4((texcoord.x - 0.5f) * 2, -(texcoord.y - 0.5f) * 2, 0, 1);
}

Texture2D colorTexture : register(t0);
SamplerState colorSampler : register(s0);

Texture2D<float> averageLuminanceMap : register(t1);

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

float3 CalculateExposure(float avgLuminance)
{
    float KeyValue = constants.KeyValue;
    // Use geometric mean
    avgLuminance = max(avgLuminance, 0.001f);
    float linearExposure = (KeyValue / avgLuminance);
    float exposure = log2(max(linearExposure, 0.0001f));
    return exp2(exposure);
}

float4 PSMain(float4 pos : SV_POSITION) : SV_Target
{
    float2 uv = pos.xy;
    float3 color = colorTexture.Load(int3(uv, 0)).rgb;

    float averageLuminance = averageLuminanceMap.Load(int3(0, 0, 0));
    float exposure = CalculateExposure(averageLuminance);

    //Tone mapping
    //float3 finalColor = Uncharted2ToneMapping(color, exposure);
    float3 finalColor = ReinhardToneMapping(color, exposure);

    //Gamma correction
    finalColor = pow(finalColor, 0.45);

    return float4(finalColor, 1.0f);
}