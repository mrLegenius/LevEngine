#include "ParticlesCommon.hlsl"

StructuredBuffer<Particle> Particles : register(t0);
StructuredBuffer<float2> SortedParticles : register(t2);

struct VertexInput
{
    uint VertexID : SV_VertexID;
};

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR;
    float Size : COLOR1;
    uint TextureIndex : COLOR2;
	float Distance : COLOR3;
};

struct PixelOutput
{
    float4 Color : SV_TARGET0;
};

PixelInput VSMain(VertexInput input)
{
    PixelInput output;

	float2 particleInfo = SortedParticles[input.VertexID];
    uint particleIndex = particleInfo.x;
	float distance = particleInfo.y;

    Particle particle = Particles[particleIndex];

    float4 worldPosition = float4(particle.Position, 1);
    float4 viewPosition = mul(worldPosition, View);
    output.Position = viewPosition;
    output.UV = 0;
    output.Color = particle.Color;
    output.Size = particle.Size;
    output.TextureIndex = particle.TextureIndex;
    output.Distance = distance;

    return output;
}

PixelInput _offsetNprojected(PixelInput data, float2 offset, float2 uv)
{
    data.Position.xy += offset;
    data.Position = mul(data.Position, Projection);
    data.UV = uv;

    return data;
}

[maxvertexcount(4)]
void GSMain(point PixelInput input[1], inout TriangleStream<PixelInput> stream)
{
    PixelInput pointOut = input[0];

    const float size = pointOut.Size;

	if (pointOut.Distance > size) return;

    stream.Append(_offsetNprojected(pointOut, float2(-1, -1) * size, float2(0, 0)));
    stream.Append(_offsetNprojected(pointOut, float2(-1, 1) * size, float2(0, 1)));
    stream.Append(_offsetNprojected(pointOut, float2(1, -1) * size, float2(1, 0)));
    stream.Append(_offsetNprojected(pointOut, float2(1, 1) * size, float2(1, 1)));

    stream.RestartStrip();
}

Texture2D ParticleTexture0 : register(t1);
Texture2D ParticleTexture1 : register(t2);
Texture2D ParticleTexture2 : register(t3);
Texture2D ParticleTexture3 : register(t4);
Texture2D ParticleTexture4 : register(t5);
Texture2D ParticleTexture5 : register(t6);

SamplerState ParticleSampler0 : register(s1);
SamplerState ParticleSampler1 : register(s2);
SamplerState ParticleSampler2 : register(s3);
SamplerState ParticleSampler3 : register(s4);
SamplerState ParticleSampler4 : register(s5);
SamplerState ParticleSampler5 : register(s6);

PixelOutput PSMain(PixelInput input)
{
    PixelOutput output;
    float4 particle;

    switch (input.TextureIndex)
    {
    default:
    case 0:
        particle = ParticleTexture0.Sample(ParticleSampler0, input.UV); break;
    case 1:
        particle = ParticleTexture1.Sample(ParticleSampler1, input.UV); break;
    case 2:
        particle = ParticleTexture2.Sample(ParticleSampler2, input.UV); break;
    case 3:
        particle = ParticleTexture3.Sample(ParticleSampler3, input.UV); break;
    case 4:
        particle = ParticleTexture4.Sample(ParticleSampler4, input.UV); break;
    case 5:
        particle = ParticleTexture5.Sample(ParticleSampler5, input.UV); break;
    }
    //particle.a = particle.r;
    output.Color = particle * input.Color;

    return output;
}