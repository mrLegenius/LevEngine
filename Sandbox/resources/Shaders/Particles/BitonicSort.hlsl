#include "ParticlesCommon.hlsl"

#define BITONIC_BLOCK_SIZE 512

cbuffer cb : register(b0)
{
	uint _Level;
	uint _LevelMask;
	uint _Width;
	uint _Height;
};

RWStructuredBuffer<float2> Data  : register(u0);

groupshared float2 sharedData[BITONIC_BLOCK_SIZE];

bool Compare(float2 left, float2 right) {
	//return (left.x == right.x) ? (left.y <= right.y) : (left.x <= right.x);
	return left.y <= right.y;
}

[numthreads(BITONIC_BLOCK_SIZE, 1, 1)]
void CSMain(uint3 Gid  : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint  GI : SV_GroupIndex) {
	// Load shared data
	sharedData[GI] = Data[DTid.x];
	GroupMemoryBarrierWithGroupSync();

	// Sort the shared data
	for (uint j = _Level >> 1; j > 0; j >>= 1) {
		float2 result;
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


