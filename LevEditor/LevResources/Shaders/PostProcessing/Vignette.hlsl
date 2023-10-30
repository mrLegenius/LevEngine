#include "../Quad.hlsl"
#include "PostProcessing.hlsl"

float4 PSMain(PS_IN input) : SV_Target
{
    float len = distance(input.uv, constants.VignetteCenter) * constants.VignetteIntensity;
    float2 radius = constants.VignetteRadius;
    float2 softness = constants.VignetteSoftness;
    float vignette = smoothstep(radius - softness, radius, len);
    float4 color = float4(constants.VignetteColor.rgb, vignette);
    return color;
}