#include "ParticlesCommon.hlsl"
#include "Random.hlsl"

#define THREAD_GROUP_SIZE 64

float Lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

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
        {
            float delta = rng.GetRandomFloat(0, 1);
            return float4(Lerp(From.x, To.x, delta), Lerp(From.y, To.y, delta), Lerp(From.z, To.z, delta), Lerp(From.w, To.w, delta));
        }
		else
            return From;
        }
    };

cbuffer Emitter : register(CB_PARTICLE_EMITTER)
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
		float GravityScale;

		//<--- 16 byte ---<<
	};

	BirthParams Birth;
};

cbuffer RandomData : register(CB_PARTICLE_RANDOM)
{
	uint RandomSeed;
	uint ParticlesToEmit;
};

cbuffer DeadParticlesCountBuffer : register(CB_PARTICLE_DEAD)
{
	uint DeadParticlesCount;
};

RWStructuredBuffer<Particle> Particles : register(U_PARTICLES);
ConsumeStructuredBuffer<uint> DeadParticles : register(U_DEAD_PARTICLES);

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
	// Bounded by both: the tail thread group overshoots ParticlesToEmit, and there may be
	// fewer free slots than we want to emit into.
	if (DTid.x >= min(ParticlesToEmit, DeadParticlesCount)) return;

	NumberGenerator random{};
	random.SetSeed(RandomSeed + DTid.x + 1u);

	Particle particle;

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

	const uint index = DeadParticles.Consume();
	Particles[index] = particle;
}
