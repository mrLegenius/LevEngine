#define CASCADE_COUNT 4

cbuffer ModelConstantBuffer : register(b1)
{
    row_major matrix model;
};

cbuffer lightSpaceConstantBuffer : register(b3)
{
    row_major matrix lightViewProjection[CASCADE_COUNT];
    float distances[CASCADE_COUNT];
};

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct GS_IN
{
    float4 pos : POSITION;
};

GS_IN VSMain(VS_IN input)
{
    GS_IN output;

    output.pos = mul(float4(input.pos, 1.0f), model);

    return output;
}

struct GS_OUT
{
    float4 pos : SV_POSITION;
    uint arrInd : SV_RenderTargetArrayIndex;
};

[instance(4)]
[maxvertexcount(3)]
void GSMain(triangle GS_IN p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
    [unroll]
    for (int i = 0; i < 3; i++)
    {
        GS_OUT gs;

        gs.pos = mul(float4(p[i].pos.xyz, 1.0f), lightViewProjection[id]);
        gs.arrInd = id;
        stream.Append(gs);
    }
}