#ifndef LEV_SHADER_COMMON_HLSL
#define LEV_SHADER_COMMON_HLSL

#include "Registers.hlsli"

#define CASCADE_COUNT 4

#ifdef WITH_ANIMATIONS
#define MAX_BONES 100
#endif

struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;

#ifdef WITH_ANIMATIONS
    int4 boneIds : BONEIDS;
    float4 boneWeights : BONEWEIGHTS;
#endif

#ifdef WITH_INSTANCING
	uint instanceId : SV_InstanceID;
#endif

};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float3x3 TBN : NORMAL;
	float2 uv : UV;
	float3 fragPos : POSITION0;
	float depth : TEXCOORD1;
};

// Vertex output for the passes that only need a position -- the deferred lighting volumes.
// Kept separate from PS_IN so those shaders do not ship four uninitialized interpolators.
struct POSITION_ONLY_PS_IN
{
	float4 pos : SV_POSITION;
	float3 fragPos : POSITION0;
	float depth : TEXCOORD1;
};

cbuffer CameraConstantBuffer : register(CB_CAMERA)
{
	row_major matrix cameraView;
	row_major matrix viewProjection;
	float3 cameraPosition;
};

cbuffer ModelConstantBuffer : register(CB_MODEL)
{
	row_major matrix model;
	row_major matrix transposedInvertedModel;

#ifdef WITH_ANIMATIONS
	row_major matrix finalBonesMatrices[MAX_BONES];
#endif
};

#ifdef WITH_INSTANCING

// One entry per instance, uploaded once per instanced draw by Renderer3D::DrawMeshInstanced.
// The whole point of instancing is that nothing is bound between instances, so the model matrix
// comes from here instead of ModelConstantBuffer, which stays unreferenced in this variant.
struct InstanceData
{
	row_major matrix Model;
	row_major matrix TransposedInvertedModel;
};

StructuredBuffer<InstanceData> instances : register(T_INSTANCE_DATA);

#define GET_MODEL(input) instances[input.instanceId].Model
#define GET_TRANSPOSED_INVERTED_MODEL(input) instances[input.instanceId].TransposedInvertedModel

#else

#define GET_MODEL(input) model
#define GET_TRANSPOSED_INVERTED_MODEL(input) transposedInvertedModel

#endif

cbuffer lightSpaceConstantBuffer : register(CB_LIGHT_SPACE)
{
	row_major matrix lightViewProjection[CASCADE_COUNT];
	float4 distances;
	float shadowMapDimensions;
};

cbuffer ScreenToViewParams : register(CB_SCREEN_TO_VIEW)
{
	float4x4 CameraInverseProjection;
	float2 ScreenDimensions;
}

Texture2DArray shadowMapTexture : register(T_SHADOW_MAP);
SamplerComparisonState shadowMapSampler : register(S_SHADOW_MAP);


float4 ClipToView(float4 clip)
{
	// View space position
	float4 view = mul(CameraInverseProjection, clip);
	// Perspective projection
	view = view / view.w;

	return view;
}

float4 ScreenToView(float4 screen)
{
	// Convert to normalized texture coordinates
	float2 texCoord = screen.xy / ScreenDimensions;

	// Convert to clip space
	float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);

	return ClipToView(clip);
}

float GetCascadeIndex(float depth)
{
	float absDepth = abs(depth);
	for (float i = 0; i < CASCADE_COUNT; i++)
	{
		if (absDepth < distances[i])
			return i;
	}

	return CASCADE_COUNT - 1;
}

// Returns the fraction of the PCF neighborhood that is lit, in 0:1.
// NdotL is deliberately NOT folded in here -- every caller runs a BRDF that already applies it.
float CalcShadow(float4 lpos, float3 normal, float3 lightDir, float cascade)
{
	//re-homogenize position after interpolation
	lpos.xyz /= lpos.w;

	//outside the light frustum there is no shadow information, so leave the fragment lit
	//rather than blacking out everything past the last cascade
	if (lpos.x < -1.0f || lpos.x > 1.0f ||
		lpos.y < -1.0f || lpos.y > 1.0f ||
		lpos.z < 0.0f || lpos.z > 1.0f) return 1.0f;

	//transform clip space coords to texture space coords (-1:1 to 0:1)
	lpos.x = lpos.x * 0.5f + 0.5f;
	lpos.y = lpos.y * -0.5f + 0.5f;

	const float ndotl = saturate(dot(normal, lightDir));

	//slope-scaled bias: a texel covers more depth the closer the surface gets to edge-on,
	//so the offset has to grow with the angle, not shrink
	const float shadowMapBias = clamp(0.0005f * tan(acos(ndotl)), 0.0005f, 0.005f);
	lpos.z -= shadowMapBias;

	const float texelSize = 1.0f / shadowMapDimensions;

	//PCF filtering on a 4 x 4 texel neighborhood
	float sum = 0;
	for (float y = -1.5; y <= 1.5; y += 1.0)
	{
		for (float x = -1.5; x <= 1.5; x += 1.0)
		{
			sum += shadowMapTexture.SampleCmpLevelZero(shadowMapSampler, float3(lpos.x + x * texelSize, lpos.y + y * texelSize, cascade), lpos.z);
		}
	}

	return sum / 16.0f;
}

float2 ApplyTextureProperties(float2 uv, float2 tiling, float2 offset)
{
    return uv * tiling + offset;
}

float3 CombineColorAndTexture(float3 color, Texture2D tex, SamplerState sampl, float2 uv)
{
	float3 texColor = tex.Sample(sampl, uv).rgb;
    float3 result = color * texColor;

    return result;
}

float3 CalculateNormal(Texture2D normalMap, SamplerState normalMapSampler, float2 uv, float3x3 TBN)
{
	float3 mapNormal = normalMap.Sample(normalMapSampler, uv).rgb;
	mapNormal = mapNormal * 2.0 - 1.0;
	return normalize(mul(mapNormal, TBN));
}

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


struct VertexCalculationResult
{
	float4 pos;
	float3 normal;
	float3 tangent;
};

VertexCalculationResult CalculateVertex(VS_IN input)
{
	VertexCalculationResult result;

#ifdef WITH_ANIMATIONS

	row_major matrix boneTransform = CalculateBoneTransform(input.boneIds, input.boneWeights);

	result.pos = mul(float4(input.pos, 1.0f), boneTransform);
	result.normal = mul(mul(float4(input.normal, 0.0f), boneTransform), GET_TRANSPOSED_INVERTED_MODEL(input)).xyz;
	result.tangent = mul(mul(float4(input.tangent, 0.0f), boneTransform), GET_TRANSPOSED_INVERTED_MODEL(input)).xyz;

#else

	result.pos = float4(input.pos, 1.0f);
	result.normal = mul(float4(input.normal, 0.0f), GET_TRANSPOSED_INVERTED_MODEL(input)).xyz;
	result.tangent = mul(float4(input.tangent, 0.0f), GET_TRANSPOSED_INVERTED_MODEL(input)).xyz;

#endif

	return result;
}

#endif
