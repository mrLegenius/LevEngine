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

struct SortedElement
{
	uint index;
	float depth;
};

RWStructuredBuffer<Particle> Particles : register(u0);
AppendStructuredBuffer<uint> DeadParticles : register(u1);
AppendStructuredBuffer<SortedElement> SortedParticles : register(u2);

#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 24
#define THREAD_GROUP_TOTAL 768

float3 _calculate(float3 anchor, float3 position)
{
	float3 direction = anchor - position;
	float distance = length(direction);
	direction /= distance;
	return direction * clamp((1 / (distance * distance)), 0.01, 1);
}

[numthreads(THREAD_GROUP_X, THREAD_GROUP_Y, 1)]
void CSMain(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
	const uint index = groupID.x * THREAD_GROUP_TOTAL + groupID.y * GroupDim * THREAD_GROUP_TOTAL + groupIndex;

	[flatten]
	if (index >= MaxParticles)
		return;

	Particle particle = Particles[index];

	[branch]
	if (particle.Age <= 0.0f)
		return;

	particle.Age -= DeltaTime;

	[branch]
	if (particle.Age <= 0.0f)
	{
		DeadParticles.Append(index);
		return;
	}

	//Simulate
	float3 position = particle.Position;
	float3 velocity = particle.Velocity;

	float3 Attractor = 0;
	//velocity += _calculate(Attractor, position);

	particle.Position = position + velocity * DeltaTime;
	particle.Velocity = velocity;

	float lifeDelta = 1 - particle.Age / particle.LifeTime;

	particle.Color = lerp(particle.StartColor, particle.EndColor, lifeDelta);
	particle.Size = lerp(particle.StartSize, particle.EndSize, lifeDelta);

	Particles[index] = particle;

	SortedElement elem;
	elem.index = index;
	elem.depth = 0;
	SortedParticles.Append(elem);
}