#ifndef LEV_PARTICLES_COMMON_HLSL
#define LEV_PARTICLES_COMMON_HLSL

#include "../Registers.hlsli"

// Mirrors GPUParticleData in Renderer/Particles/GPUParticleData.h. Structured buffers are
// tightly packed, so field order is the byte layout -- keep the two in step.
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
	float GravityScale;
};

// Mirrors SortedParticleData. The bitonic sort orders these by Depth.
struct SortedElement
{
	uint Index;
	float Depth;
};

// Depth key parked on dead particles so the sort pushes them past every live one -- real keys
// are -length(cameraToParticle) and therefore always negative. The rendering GS drops anything
// at or above this.
static const float k_DeadParticleDepth = 1e6f;

cbuffer CameraParams : register(CB_PARTICLE_CAMERA)
{
	row_major matrix View;
	row_major matrix Projection;
	float3 Position;
};

cbuffer Handler : register(CB_PARTICLE_HANDLER)
{
	int GroupDim;
	uint MaxParticles;
	float DeltaTime;
};

#endif
