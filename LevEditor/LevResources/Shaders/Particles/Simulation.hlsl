#include "ParticlesCommon.hlsl"

RWStructuredBuffer<Particle> Particles : register(U_PARTICLES);
AppendStructuredBuffer<uint> DeadParticles : register(U_DEAD_PARTICLES);
RWStructuredBuffer<SortedElement> SortedParticles : register(U_SORTED_PARTICLES);

#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 32
#define THREAD_GROUP_TOTAL 1024

Texture2D normalTexture : register(T_PARTICLE_NORMAL);
Texture2D depthTexture : register(T_PARTICLE_DEPTH);

SortedElement MakeSortedElement(uint index, float depth)
{
	SortedElement element;
	element.Index = index;
	element.Depth = depth;
	return element;
}

// Bounces the particle off whatever the G-buffer says is at its screen position. Returns the
// velocity unchanged when the particle is off screen or the surface behind it is too far away.
float3 ApplyDepthBufferBounce(float3 position, float3 velocity)
{
	float4 clipPos = mul(mul(float4(position, 1.0f), View), Projection);
	clipPos /= clipPos.w;

	//no depth or normal to read outside the frustum
	if (any(abs(clipPos.xy) > 1.0f) || clipPos.z < 0.0f || clipPos.z > 1.0f)
		return velocity;

	float2 uv = (clipPos.xy + 1.0f) * 0.5f;
	uv.y = 1.0f - uv.y;

	float bufferWidth, bufferHeight;
	normalTexture.GetDimensions(bufferWidth, bufferHeight);
	int2 texel = int2(uv * float2(bufferWidth, bufferHeight));

	float surfaceDepth = depthTexture.Load(int3(texel, 0)).r;
	if (clipPos.z <= surfaceDepth || (clipPos.z - surfaceDepth) > 0.001f)
		return velocity;

	//the G-buffer normal is zero wherever nothing was drawn, and normalizing that is a NaN
	float3 normal = normalTexture.Load(int3(texel, 0)).xyz;
	if (dot(normal, normal) < 1e-6f)
		return velocity;

	return reflect(velocity, normalize(normal));
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
	if (particle.Age < 0)
	{
		SortedParticles[index] = MakeSortedElement(index, k_DeadParticleDepth);
		return;
	}

	particle.Age += DeltaTime;

	[branch]
	if (particle.Age >= particle.LifeTime)
	{
		particle.Age = -1;
		Particles[index] = particle;

		DeadParticles.Append(index);
		SortedParticles[index] = MakeSortedElement(index, k_DeadParticleDepth);
		return;
	}

	//Simulate
	float3 position = particle.Position;
	float3 velocity = particle.Velocity;

	velocity += float3(0, -9.8f * DeltaTime * particle.GravityScale, 0);
	velocity = ApplyDepthBufferBounce(position, velocity);

	particle.Position = position + velocity * DeltaTime;
	particle.Velocity = velocity;

	float lifeDelta = particle.Age / particle.LifeTime;

	particle.Color = lerp(particle.StartColor, particle.EndColor, lifeDelta);
	particle.Size = lerp(particle.StartSize, particle.EndSize, lifeDelta);

	Particles[index] = particle;

	SortedParticles[index] = MakeSortedElement(index, -length(Position - particle.Position));
}
