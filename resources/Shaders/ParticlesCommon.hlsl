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
	float TextureIndex;
};

struct SortedElement
{
	uint index;
	float depth;
};

cbuffer Handler : register(b0)
{
	int GroupDim;
	uint MaxParticles;
	float DeltaTime;
};

