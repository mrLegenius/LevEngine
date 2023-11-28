#include "Common.hlsli"

Texture2D equirectangularMap;
SamplerState sam;

static const float2 invAtan = float2(0.1591, 0.3183);
float2 SampleSphericalMap(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float2 uv = SampleSphericalMap(normalize(input.uv));
    float3 color = equirectangularMap.Sample(sam, uv).rgb;
    
    return float4(color, 1.0f);
}