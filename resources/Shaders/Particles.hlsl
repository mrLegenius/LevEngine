struct Particle
{
    float3 Position;
    float3 Velocity;

    float4 StartColor;
    float4 EndColor;
    float4 Color;

    float StartSize;
    float EndSize;
    float Size;

    float Age;
    float LifeTime;
};

struct SortedElement
{
    uint index;
    float depth;
};

StructuredBuffer<Particle> Particles : register(t0); // ����� ������
StructuredBuffer<SortedElement> SortedParticles : register(t2); // ����� ������

cbuffer Params : register(b0) // ������� ���� � ��������
{
    row_major matrix View;
    row_major matrix Projection;
};

// �.�. ��������� � ��� ���, �� ����� �������� ������� ID �������� ��� ��������� ��� ������������� Vertex Buffer
struct VertexInput
{
    uint VertexID : SV_VertexID;
};

struct PixelInput // ��������� ������� �� ������ �� Vertex Shader
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR;
    float Size : COLOR1;
};

struct PixelOutput // ���� ��������������� �������
{
    float4 Color : SV_TARGET0;
};

PixelInput VSMain(VertexInput input)
{
    PixelInput output;

    uint particleIndex = SortedParticles[input.VertexID].index;
    Particle particle = Particles[particleIndex];

    float4 worldPosition = float4(particle.Position, 1);
    float4 viewPosition = mul(worldPosition, View);
    output.Position = viewPosition;
    output.UV = 0;
    output.Color = particle.Color;
    output.Size = particle.Size;

    return output;
}

// ������� ��������� �������� � ����������� �������� ��� � Projection Space
PixelInput _offsetNprojected(PixelInput data, float2 offset, float2 uv)
{
    data.Position.xy += offset;
    data.Position = mul(data.Position, Projection);
    data.UV = uv;

    return data;
}

[maxvertexcount(4)] // ��������� ������ GS � 4 ��������, ������� �������� TriangleStrip
void GSMain(point PixelInput input[1], inout TriangleStream<PixelInput> stream)
{
    PixelInput pointOut = input[0];

    const float size = pointOut.Size;

    // �������� ��������
    stream.Append(_offsetNprojected(pointOut, float2(-1, -1) * size, float2(0, 0)));
    stream.Append(_offsetNprojected(pointOut, float2(-1, 1) * size, float2(0, 1)));
    stream.Append(_offsetNprojected(pointOut, float2(1, -1) * size, float2(1, 0)));
    stream.Append(_offsetNprojected(pointOut, float2(1, 1) * size, float2(1, 1)));

    // ������� TriangleStrip
    stream.RestartStrip();
}

Texture2D ParticleTexture : register(t1);
SamplerState ParticleSampler : register(s1);

PixelOutput PSMain(PixelInput input)
{
    PixelOutput output;

    float4 particle = ParticleTexture.Sample(ParticleSampler, input.UV);
    //particle.a = particle.r;
    output.Color = particle * input.Color;

    return output;
}