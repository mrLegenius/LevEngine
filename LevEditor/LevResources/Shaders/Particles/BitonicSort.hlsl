#include "ParticlesCommon.hlsl"

#define BITONIC_BLOCK_SIZE 1024

cbuffer cb : register(CB_PARTICLE_SORT)
{
	uint _Level;
	uint _LevelMask;
	uint _Width;
	uint _Height;
};

RWStructuredBuffer<SortedElement> Data  : register(U_PARTICLES);

groupshared SortedElement sharedData[BITONIC_BLOCK_SIZE];

bool Compare(SortedElement left, SortedElement right) {
	return left.Depth <= right.Depth;
}

[numthreads(BITONIC_BLOCK_SIZE, 1, 1)]
void CSMain(uint3 Gid  : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint  GI : SV_GroupIndex) {
	// Load shared data
	sharedData[GI] = Data[DTid.x];
	GroupMemoryBarrierWithGroupSync();

	// Sort the shared data
	for (uint j = _Level >> 1; j > 0; j >>= 1) {
		SortedElement result;
		if (Compare(sharedData[GI & ~j], sharedData[GI | j]) == (bool)(_LevelMask & DTid.x))
			result = sharedData[GI ^ j];
		else
			result = sharedData[GI];
		GroupMemoryBarrierWithGroupSync();
		sharedData[GI] = result;
		GroupMemoryBarrierWithGroupSync();
	}

	// Store shared data
	Data[DTid.x] = sharedData[GI];
}
