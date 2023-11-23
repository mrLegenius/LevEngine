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

    return output;
}

//<--- Geometry Shader ---<<

struct GS_OUT
{
    float4 pos : SV_Position;
    uint slice : SV_RenderTargetArrayIndex;
    float3 uv : UV;
};

[instance(6)]
[maxvertexcount(3)]
void GSMain(triangle VS_OUT p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
    [unroll]
    for (int i = 0; i < 3; i++)
    {
        GS_OUT gs;

        gs.pos = mul(float4(p[i].pos.xyz, 1.0f), cubeMapViewProjections[id]);
        gs.slice = id;
        gs.uv = p[i].uv;
        stream.Append(gs);
    }
}

//<--- Pixel Shader ---<<

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

float4 PSMain(GS_OUT input) : SV_Target
{
    float2 uv = SampleSphericalMap(normalize(input.uv));
    float3 color = equirectangularMap.Sample(sam, uv).rgb;
    
    return float4(color, 1.0f);
}