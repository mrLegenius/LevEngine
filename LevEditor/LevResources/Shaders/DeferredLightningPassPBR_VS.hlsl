#include "ShaderCommon.hlsl"

POSITION_ONLY_PS_IN VSMain(VS_IN input)
{
    POSITION_ONLY_PS_IN output;

    float4 fragPos = mul(float4(input.pos, 1.0f), model);

    output.pos = mul(fragPos, viewProjection);
    output.fragPos = fragPos.xyz;
    output.depth = mul(fragPos, cameraView).z;

    return output;
}
