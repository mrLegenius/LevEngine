#include "ParticlesCommon.hlsl"
#include "Random.hlsl"

struct RandomFloat
{
	float From;
	float To;
	bool Randomize;
	float padding;

	float GetRandom(NumberGenerator rng)
	{
		if (Randomize)
			return rng.GetRandomFloat(From, To);
		else
			return From;
	}
};

struct RandomFloat2
{
	float2 From;
	float2 To;
	bool Randomize;
	float padding;
	float padding1;
	float padding2;

	float2 GetRandom(NumberGenerator rng)
	{
		if (Randomize)
			return float2(rng.GetRandomFloat(From.x, To.x), rng.GetRandomFloat(From.y, To.y));
		else
			return From;
	}
};

struct RandomFloat3
{
	float3 From;
	float3 To;
	bool Randomize;

	float3 GetRandom(NumberGenerator rng)
	{
		if (Randomize)
			return float3(rng.GetRandomFloat(From.x, To.x), rng.GetRandomFloat(From.y, To.y), rng.GetRandomFloat(From.z, To.z));
		else
			return From;
	}
};

struct RandomFloat4
{
	float4 From;
	float4 To;
	bool Randomize;

	float padding;
	float padding2;
	float padding3;

	float4 GetRandom(NumberGenerator rng)
	{
		if (Randomize)
			return float4(rng.GetRandomFloat(From.x, To.x), rng.GetRandomFloat(From.y, To.y), rng.GetRandomFloat(From.z, To.z), rng.GetRandomFloat(From.w, To.w));
		else
			return From;
	}
};

cbuffer Emitter : register(b2)
{
	struct BirthParams
	{
		RandomFloat3 Velocity;
		RandomFloat3 Position;

		RandomFloat4 StartColor;
		float4 EndColor;

		//<--- 16 byte ---<<
		RandomFloat StartSize;
		//<--- 16 byte ---<<

		float EndSize;

		RandomFloat LifeTime;
		uint TextureIndex;

		//<--- 16 byte ---<<
		float GravityScale;
	};
	
	BirthParams Birth;
};

RWStructuredBuffer<Particle> Particles : register(u0);
ConsumeStructuredBuffer<uint> DeadParticles : register(u1);

[numthreads(1, 1, 1)]
void CSMain(uint3 groupID : SV_GroupID)
{
	const uint index = DeadParticles.Consume();

	if (index >= MaxParticles) return;

	NumberGenerator random;
	random.SetSeed(RandomSeed);
	int cycleCount = (groupID.x + 1) * (groupID.y + 1) * (groupID.z + 1);
	random.Cycle(cycleCount);

	Particle particle = Particles[index];

	particle.TextureIndex = Birth.TextureIndex;

	particle.Position = Birth.Position.GetRandom(random);
	particle.Velocity = Birth.Velocity.GetRandom(random);

	particle.Age = 0;
	particle.LifeTime = Birth.LifeTime.GetRandom(random);

	particle.StartColor = Birth.StartColor.GetRandom(random);
	particle.EndColor = Birth.EndColor;
	particle.Color = particle.StartColor;

	particle.StartSize = Birth.StartSize.GetRandom(random);
	particle.EndSize = Birth.EndSize;
	particle.Size = particle.StartSize;

	particle.GravityScale = Birth.GravityScale;

	Particles[index] = particle;
}