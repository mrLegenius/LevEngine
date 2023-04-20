#include "ParticlesCommon.hlsl"

RWStructuredBuffer<Particle> Particles : register(u0);
AppendStructuredBuffer<uint> DeadParticles : register(u1);
AppendStructuredBuffer<SortedElement> SortedParticles : register(u2);

#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 32
#define THREAD_GROUP_TOTAL 1024

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
	if (particle.Age >= particle.LifeTime)
		return;

	particle.Age += DeltaTime;

	[branch]
	if (particle.Age >= particle.LifeTime || particle.Age < 0)
	{
		DeadParticles.Append(index + 1);
		return;
	}

	//Simulate
	float3 position = particle.Position;
	float3 velocity = particle.Velocity;

	float3 Attractor = 0;
	//velocity += _calculate(Attractor, position);

	particle.Position = position + velocity * DeltaTime;
	particle.Velocity = velocity;

	float lifeDelta = particle.Age / particle.LifeTime;

	particle.Color = lerp(particle.StartColor, particle.EndColor, lifeDelta);
	particle.Size = lerp(particle.StartSize, particle.EndSize, lifeDelta);

	Particles[index] = particle;

	SortedElement elem;
	elem.index = index;
	elem.depth = 0;
	SortedParticles.Append(elem);
}