#include "ParticlesCommon.hlsl"

cbuffer Emitter : register(b2)
{
	struct BirthParams
	{
		float3 Velocity;
		float3 Position;

		float4 StartColor;
		float4 EndColor;

		float StartSize;
		float EndSize;

		float LifeTime;
		uint TextureIndex;
	};
	
	BirthParams Birth;
};

RWStructuredBuffer<Particle> Particles : register(u0);
ConsumeStructuredBuffer<uint> DeadParticles : register(u1);

[numthreads(1, 1, 1)]
void CSMain()
{
	const uint index = DeadParticles.Consume();

	if (index >= MaxParticles) return;

	Particle particle = Particles[index];

	particle.TextureIndex = Birth.TextureIndex;

	particle.Position = Birth.Position;
	particle.Velocity = Birth.Velocity;

	particle.Age = 0;
	particle.LifeTime = Birth.LifeTime;

	particle.StartColor = Birth.StartColor;
	particle.EndColor = Birth.EndColor;
	particle.Color = Birth.StartColor;

	particle.StartSize = Birth.StartSize;
	particle.EndSize = Birth.EndSize;
	particle.Size = Birth.StartSize;

	Particles[index] = particle;
}