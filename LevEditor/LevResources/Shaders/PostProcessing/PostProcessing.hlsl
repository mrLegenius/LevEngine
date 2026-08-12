#ifndef LEV_POST_PROCESSING_HLSL
#define LEV_POST_PROCESSING_HLSL

#include "../Registers.hlsli"

struct Constants
{
    float BloomThreshold;
    float BloomMagnitude;
    float BloomBlurSigma;
    float Tau;

    float TimeDelta;
    float KeyValue;
	float MinExposure;
	float MaxExposure;

    float4 VignetteColor;

    float2 VignetteCenter;
    float VignetteRadius;
    float VignetteSoftness;
            
    float VignetteIntensity;
};

cbuffer PostProcessingParams : register(CB_POST_PROCESSING)
{
    Constants constants;
}

Texture2D colorTexture : register(t0);

SamplerState linearSampler : register(s0);
SamplerState pointSampler : register(s1);

// Approximates luminance from an RGB value
float CalcLuminance(float3 color)
{
    return max(dot(color, float3(0.299f, 0.587f, 0.114f)), 0.0001f);
}

float CalculateExposure(float avgLuminance, float threshold)
{
    // Use geometric mean
    avgLuminance = max(avgLuminance, 0.001f);
    float linearExposure = (constants.KeyValue / avgLuminance);
    float exposure = log2(max(linearExposure, 0.0001f));
    exposure -= threshold;
    return exp2(exposure);
}

float CalculateExposure(float avgLuminance)
{
	return CalculateExposure(avgLuminance, 0);
}

float CalcGaussianWeight(int sampleDist, float sigma)
{
    float g = 1.0f / sqrt(2.0f * 3.14159 * sigma * sigma);
    return (g * exp(-(sampleDist * sampleDist) / (2 * sigma * sigma)));
}

// Performs a gaussian blur in one direction
float4 Blur(float2 uv, float2 texScale, float sigma)
{
    float width;
    float height;
    colorTexture.GetDimensions(width, height);

    float4 color = 0;
    float totalWeight = 0;

    //the range has to be symmetric, otherwise every blur pass drags the image half a texel
    for (int i = -6; i <= 6; i++)
    {
        float weight = CalcGaussianWeight(i, sigma);
        float2 texCoord = uv;
        texCoord += (i / float2(width, height)) * texScale;
        float4 sample = colorTexture.Sample(linearSampler, texCoord);
        color += sample * weight;
        totalWeight += weight;
    }

    //a gaussian truncated at +-6 texels does not sum to 1, and falls further short as sigma
    //grows -- without this the bloom dims as BloomBlurSigma rises
    return color / totalWeight;
}

#endif