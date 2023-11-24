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

TextureCube environmentMap;
SamplerState Sampler;

static const float PI = 3.14159265359f;

float4 PSMain(GS_OUT input) : SV_Target
{
    float3 normal = normalize(input.uv);
    float3 irradiance = float3(0.0f, 0.0f, 0.0f);

    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    float sampleDelta = 0.025f;
    float nrSamples = 0.0f;
    for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
    {
        for(float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

            irradiance += environmentMap.Sample(Sampler, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0f / float(nrSamples));
    
    return float4(irradiance, 1.0f);
}