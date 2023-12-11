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
	float GravityScale;
};

struct SortedElement
{
	uint index;
	float depth;
};

cbuffer CameraParams : register(b0)
{
	row_major matrix View;
	row_major matrix Projection;
	float3 Position;
};

cbuffer Handler : register(b1)
{
	int GroupDim;
	uint MaxParticles;
	float DeltaTime;
};