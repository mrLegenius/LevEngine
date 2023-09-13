#define CASCADE_COUNT 4

cbuffer CameraConstantBuffer : register(b0)
{
    row_major matrix viewProjection;
    float3 cameraPosition;
};

cbuffer ModelConstantBuffer : register(b1)
{
    row_major matrix model;
};

cbuffer lightSpaceConstantBuffer : register(b3)
{
    row_major matrix lightViewProjection;
};

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output;
    // Transform the vertex position into projected space.
    float4 pos = float4(input.pos, 1.0f);
    matrix mvp = mul(model, lightViewProjection);
    output.pos = mul(pos, mvp);

    return output;
}

float PSMain(PS_IN input) : SV_Depth
{
    return input.pos.z;
}