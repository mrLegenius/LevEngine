#include "Common.hlsli"

//<--- Vertex Shader ---<<

cbuffer cubeMapConstantBuffer : register(b6)
{
    row_major matrix cubeMapViewProjections[6];
};

struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float3 uv : UV;
};

VS_OUT VSMain(VS_IN input)
{
    VS_OUT output;

    output.pos = float4(input.pos, 1.0f);
    output.uv = input.pos;
    output.uv.z = -input.pos.z;

    return output;
}

//<--- Geometry Shader ---<<

[instance(6)]
[maxvertexcount(3)]
void GSMain(triangle VS_OUT p[3], in uint id : SV_GSInstanceID, inout TriangleStream<PS_IN> stream)
{
    [unroll]
    for (int i = 0; i < 3; i++)
    {
        PS_IN output;

        output.pos = mul(float4(p[i].pos.xyz, 1.0f), cubeMapViewProjections[id]);
        output.slice = id;
        output.uv = p[i].uv;
        stream.Append(output);
    }
}

//TODO: What is faster Instancing or triangle generation

[maxvertexcount(18)]
void GSMain2(triangle VS_OUT p[3], inout TriangleStream<PS_IN> stream)
{
    [unroll]
    for (int i = 0; i < 6; i++)
    {
        PS_IN output;
        output.slice = i;

        [unroll]
        for( int v = 0; v < 3; ++v )
        {
            output.pos = mul(p[v].pos, cubeMapViewProjections[i]);
            output.uv = p[v].uv;
            stream.Append(output);
        }

        stream.RestartStrip();
    }
}
