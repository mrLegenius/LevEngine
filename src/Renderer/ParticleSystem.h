#pragma once
#include "D3D11StructuredBuffer.h"
#include "RenderParams.h"
#include "Kernel/PointerUtils.h"

struct GPUParticleData
{
	Vector3 Position;
	Vector3 Velocity;

	Color StartColor;
	Color EndColor;
	Color Color;

	float StartSize;
	float EndSize;
	float Size;

	float LifeTime;
	float Age;
	uint32_t TextureIndex;
};

class ParticleSystem
{
public:
	explicit ParticleSystem(uint32_t maxParticles)
	{
		
	}

	

private:

};
