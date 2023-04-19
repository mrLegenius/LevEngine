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

cbuffer Handler : register(b0)
{
	int GroupDim;
	uint MaxParticles;
	float DeltaTime;
};

cbuffer Emitter : register(b1)
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

	particle.Position = Birth.Position;
	particle.Velocity = Birth.Velocity;

	particle.Age = Birth.LifeTime;
	particle.LifeTime = Birth.LifeTime;

	particle.StartColor = Birth.StartColor;
	particle.EndColor = Birth.EndColor;
	particle.Color = Birth.StartColor;

	particle.StartSize = Birth.StartSize;
	particle.EndSize = Birth.EndSize;
	particle.Size = Birth.StartSize;

	Particles[index] = particle;
}