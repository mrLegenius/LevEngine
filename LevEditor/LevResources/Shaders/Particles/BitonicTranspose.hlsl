#define TRANSPOSE_BLOCK_SIZE 32

cbuffer cb : register(b0)
{
	uint _Level;
	uint _LevelMask;
	uint _Width;
	uint _Height;
};

StructuredBuffer<float2> Input : register(t0);
RWStructuredBuffer<float2> Data  : register(u0);

groupshared float2 transpose_shared_data[TRANSPOSE_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE];

[numthreads(TRANSPOSE_BLOCK_SIZE, TRANSPOSE_BLOCK_SIZE, 1)]
void CSMain(uint3 Gid  : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex) 
{
	transpose_shared_data[GI] = Input[DTid.y * _Width + DTid.x];
	GroupMemoryBarrierWithGroupSync();
	uint2 XY = DTid.yx - GTid.yx + GTid.xy;
	Data[XY.y * _Height + XY.x] = transpose_shared_data[GTid.x * TRANSPOSE_BLOCK_SIZE + GTid.y];
}