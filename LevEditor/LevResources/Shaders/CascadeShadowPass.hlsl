#define CASCADE_COUNT 4

#ifdef WITH_ANIMATIONS
#define MAX_BONES 100
#endif

cbuffer ModelConstantBuffer : register(b1)
{
    row_major matrix model;
#ifdef WITH_ANIMATIONS
	row_major matrix transposedInvertedModel;
	row_major matrix finalBonesMatrices[MAX_BONES];
#endif
};

cbuffer lightSpaceConstantBuffer : register(b3)
{
    row_major matrix lightViewProjection[CASCADE_COUNT];
    float4 distances;
};

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
#ifdef WITH_ANIMATIONS
	int4 boneIds : BONEIDS;
    float4 boneWeights : BONEWEIGHTS;
#endif
};

struct GS_IN
{
    float4 pos : POSITION;
};

#ifdef WITH_ANIMATIONS
row_major matrix CalculateBoneTransform(int4 boneIds, float4 boneWeights)
{
    row_major matrix boneTransform = matrix(
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0
        );

    boneTransform += mul(boneWeights[0], finalBonesMatrices[boneIds[0]]);
    boneTransform += mul(boneWeights[1], finalBonesMatrices[boneIds[1]]);
    boneTransform += mul(boneWeights[2], finalBonesMatrices[boneIds[2]]);
    boneTransform += mul(boneWeights[3], finalBonesMatrices[boneIds[3]]);

    return boneTransform;
}
#endif

GS_IN VSMain(VS_IN input)
{
    GS_IN output;

#ifdef WITH_ANIMATIONS
    row_major matrix boneTransform = CalculateBoneTransform(input.boneIds, input.boneWeights);
	
	float4 pos = mul(float4(input.pos, 1.0f), boneTransform);
	output.pos = mul(pos, model);
#else
    output.pos = mul(float4(input.pos, 1.0f), model);
#endif

    return output;
}

struct GS_OUT
{
    float4 pos : SV_POSITION;
    uint arrInd : SV_RenderTargetArrayIndex;
};

[instance(4)]
[maxvertexcount(3)]
void GSMain(triangle GS_IN p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
    [unroll]
    for (int i = 0; i < 3; i++)
    {
        GS_OUT gs;

        gs.pos = mul(float4(p[i].pos.xyz, 1.0f), lightViewProjection[id]);
        gs.arrInd = id;
        stream.Append(gs);
    }
}