#ifndef LEV_ENVIRONMENT_COMMON_HLSLI
#define LEV_ENVIRONMENT_COMMON_HLSLI

#include "../Registers.hlsli"

struct PS_IN
{
    float4 pos : SV_Position;
    uint slice : SV_RenderTargetArrayIndex;
    float3 uv : UV;
};

static const float PI = 3.14159265359f;

// Ceiling applied to environment samples during IBL precompute. High enough to keep the sky
// and sun in HDR, low enough that a single blown texel cannot dominate a whole hemisphere.
static const float k_MaxRadiance = 50.0f;

#endif