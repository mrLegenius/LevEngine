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

};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float3x3 TBN : NORMAL;
	float2 uv : UV;
	float3 fragPos : POSITION0;
	float depth : TEXCOORD1;
};

cbuffer CameraConstantBuffer : register(b0)
{
	row_major matrix cameraView;
	row_major matrix viewProjection;
	float3 cameraPosition;
};

cbuffer ModelConstantBuffer : register(b1)
{
	row_major matrix model;
	row_major matrix transposedInvertedModel;

#ifdef WITH_ANIMATIONS
	row_major matrix finalBonesMatrices[MAX_BONES];
#endif
};

//lighting cbuffer b2

cbuffer lightSpaceConstantBuffer : register(b3)
{
	row_major matrix lightViewProjection[CASCADE_COUNT];
	float4 distances;
	float shadowMapDimensions;
};

//material cbuffer b4

cbuffer ScreenToViewParams : register(b5)
{
	float4x4 CameraInverseProjection;
	float2 ScreenDimensions;
}

//skybox cbuffer at b6
//debug cbuffer at b7
//post processing cbuffer at b8

Texture2DArray shadowMapTexture : register(t9);
SamplerComparisonState shadowMapSampler : register(s9);

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

float CalcShadow(float4 lpos, float3 normal, float3 lightDir, float cascade)
{
	//re-homogenize position after interpolation
	lpos.xyz /= lpos.w;

	//if position is not visible to the light - dont illuminate it
	//results in hard light frustum
	if (lpos.x < -1.0f || lpos.x > 1.0f ||
		lpos.y < -1.0f || lpos.y > 1.0f ||
		lpos.z < 0.0f || lpos.z > 1.0f) return 0.0f;

	//transform clip space coords to texture space coords (-1:1 to 0:1)
	lpos.x = lpos.x * 0.5f + 0.5f;
	lpos.y = lpos.y * -0.5f + 0.5f;

	float ndotl = dot(normal, lightDir);

	//float shadowMapBias = max(0.005 * (1.0 - ndotl), 0.0005);
	float shadowMapBias = clamp(0.0005 / acos(saturate(ndotl)), 0, 0.005);
	//float shadowMapBias = max(0.005 * tan(acos(ndotl)), 0.0005);
	lpos.z -= shadowMapBias;

	float texelSize = 1.0f / shadowMapDimensions;

	//No filtering
	//float shadowFactor = shadowMapTexture.SampleCmpLevelZero(shadowMapSampler, float3(lpos.xy, cascade), lpos.z);

	//PCF filtering on a 4 x 4 texel neighborhood
	float sum = 0;
	for (float y = -1.5; y <= 1.5; y += 1.0)
	{
		for (float x = -1.5; x <= 1.5; x += 1.0)
		{
			sum += shadowMapTexture.SampleCmpLevelZero(shadowMapSampler, float3(lpos.x + x * texelSize, lpos.y + y * texelSize, cascade), lpos.z);
		}
	}
	float shadowFactor = sum / 16.0;


	//if clip space z value greater than shadow map value then pixel is in shadow
	if (shadowFactor < lpos.z) return 0.0f;

	//otherwise calculate ilumination at fragment
	return ndotl * shadowFactor;
}

float2 ApplyTextureProperties(float2 uv, float2 tiling, float2 offset)
{
    return uv * tiling + offset;
}

float3 CombineColorAndTexture(float3 color, Texture2D tex, SamplerState sampl, float2 uv)
{
	float3 texColor = tex.Sample(sampl, uv);
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
