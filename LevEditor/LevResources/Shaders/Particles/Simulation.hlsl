#include "ParticlesCommon.hlsl"

RWStructuredBuffer<Particle> Particles : register(u0);
AppendStructuredBuffer<uint> DeadParticles : register(u1);
RWStructuredBuffer<float2> SortedParticles : register(u2);

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

Texture2D normalTexture : register(t8);
Texture2D depthTexture : register(t9);

[numthreads(THREAD_GROUP_X, THREAD_GROUP_Y, 1)]
void CSMain(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
	const uint index = groupID.x * THREAD_GROUP_TOTAL + groupID.y * GroupDim * THREAD_GROUP_TOTAL + groupIndex;

	[flatten]
	if (index >= MaxParticles)
		return;

	Particle particle = Particles[index];

	[branch]
	if (particle.Age < 0)
	{
		particle.Color = float4(1.0f, 0.0f, 1.0f, 1.0f);
		particle.Size = 10;
		Particles[index] = particle;

		SortedParticles[index] = float2(index, 1000000);
		return;
	}

	particle.Age += DeltaTime;

	[branch]
	if (particle.Age >= particle.LifeTime)
	{
		particle.Age = -1;
		Particles[index] = particle;

		DeadParticles.Append(index);
		SortedParticles[index] = float2(index, 999999);
		return;
	}

	//Simulate
	float3 position = particle.Position;
	float3 velocity = particle.Velocity;

	float3 Attractor = 0;
	//velocity += _calculate(Attractor, position);
	velocity += float3(0, -9.8f * DeltaTime * particle.GravityScale, 0);

	{//<--- particle bounce ---<<
		float4 posViewSpace = mul(mul(float4(particle.Position, 1.0f), View), Projection);
		posViewSpace = posViewSpace / posViewSpace.w;
		float particleDepth = posViewSpace.z;

		float2 uv = (posViewSpace.xy + float2(1.0f, 1.0f)) / 2;
		uv.y = 1.0f - uv.y;

		float bufferWidth, bufferHeight;
		normalTexture.GetDimensions(bufferWidth, bufferHeight);
		uv.xy *= float2(bufferWidth, bufferHeight);

		float4 normal = normalTexture.Load(int3(uv, 0));
		float depth = depthTexture.Load(int3(uv, 0)).r;

		if (particleDepth > depth && (particleDepth - depth) <= 0.001f)
		{
			velocity = reflect(float4(velocity, 1.0f), normal);
		}
	}

	particle.Position = position + velocity * DeltaTime;
	particle.Velocity = velocity;

	float lifeDelta = particle.Age / particle.LifeTime;

	particle.Color = lerp(particle.StartColor, particle.EndColor, lifeDelta);
	particle.Size = lerp(particle.StartSize, particle.EndSize, lifeDelta);

	Particles[index] = particle;

	float depth = -length(Position - particle.Position);
	SortedParticles[index] = float2(index, depth);
	SortedParticles.IncrementCounter();
}