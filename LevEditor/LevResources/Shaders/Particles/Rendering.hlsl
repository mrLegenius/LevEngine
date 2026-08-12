#include "ParticlesCommon.hlsl"

StructuredBuffer<Particle> Particles : register(T_PARTICLE_BUFFER);
StructuredBuffer<SortedElement> SortedParticles : register(T_PARTICLE_SORTED);

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
	float Depth : COLOR2;
};

struct PixelOutput
{
    float4 Color : SV_TARGET0;
};

PixelInput VSMain(VertexInput input)
{
    PixelInput output;

	SortedElement element = SortedParticles[input.VertexID];
    Particle particle = Particles[element.Index];

    float4 worldPosition = float4(particle.Position, 1);
    float4 viewPosition = mul(worldPosition, View);
    output.Position = viewPosition;
    output.UV = 0;
    output.Color = particle.Color;
    output.Size = particle.Size;
    output.Depth = element.Depth;

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

	//dead slots carry the sentinel key the simulation parked on them; live particles always
	//have a negative depth
	if (pointOut.Depth >= k_DeadParticleDepth) return;

    stream.Append(_offsetNprojected(pointOut, float2(-1, -1) * size, float2(0, 0)));
    stream.Append(_offsetNprojected(pointOut, float2(-1, 1) * size, float2(0, 1)));
    stream.Append(_offsetNprojected(pointOut, float2(1, -1) * size, float2(1, 0)));
    stream.Append(_offsetNprojected(pointOut, float2(1, 1) * size, float2(1, 1)));

    stream.RestartStrip();
}

Texture2D ParticleTexture : register(T_PARTICLE_TEXTURE);

SamplerState ParticleSampler : register(S_PARTICLE_TEXTURE);

PixelOutput PSMain(PixelInput input)
{
    PixelOutput output;
    float4 particle = ParticleTexture.Sample(ParticleSampler, input.UV);

    output.Color = particle * input.Color;

    return output;
}
