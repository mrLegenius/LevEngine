#ifndef LEV_PLANET_COMMON_HLSLI
#define LEV_PLANET_COMMON_HLSLI

// The planet's constant buffers and the biome rule, which is a mirror of PlanetBiome.cpp.
//
// The climate itself is not evaluated here: temperature, rainfall and slope arrive interpolated in a
// vertex stream, because working them out involves a rain shadow that needs a second elevation
// sample, and a pixel shader has no business running the noise stack twice. What is done per pixel is
// the part that has to be: turning those numbers into biome weights, and blending the ground
// textures by them. A vertex cannot carry a blend of four textures, only the climate that selects it.
//
// Keep the layout in step with GPUPlanetData in PlanetConstants.h.

#include "../Registers.hlsli"
#include "Noise.hlsli"

#define PLANET_MAX_BIOMES 16
#define PLANET_MAX_LAYERS 4

//<--- Bits of TextureFlags ---<<
#define PLANET_HAS_ALBEDO 1
#define PLANET_HAS_NORMAL 2
#define PLANET_HAS_ROUGHNESS 4

struct PlanetBiomeData
{
	//<--- Celsius, then rainfall 0..1 ---<<
	float4 ClimateRange;   // tempMin, tempMax, humidityMin, humidityMax

	//<--- Width of the fade at each edge of the four windows ---<<
	float4 Blends;         // temperature, humidity, height, slope

	float4 HeightSlope;    // heightMin, heightMax, slopeMin, slopeMax

	float4 Tint;           // rgb, priority

	float4 Surface;        // roughness, metallic, textureScale, textureIndex

	float4 Detail;         // normalStrength, unused, unused, unused
};

cbuffer PlanetConstantBuffer : register(CB_PLANET)
{
	//<--- Sea level radius, and the range the surface strays from it ---<<
	float PlanetSurfaceRadius;
	float PlanetElevationRange;
	int PlanetBiomeCount;
	int PlanetTextureFlags;

	// Detail below the size of a vertex, evaluated here rather than baked into the mesh. This is the
	// only reason the noise has a shader mirror at all: the vertices carry the shape down to their
	// own spacing, and this carries it the rest of the way to the pixel.
	float PlanetDetailStrength;
	float PlanetDetailFrequency;
	uint PlanetDetailSeed;
	float PlanetTriplanarSharpness;

	//<--- How far a pixel may be before the detail is not worth evaluating, in world units ---<<
	float PlanetDetailFadeStart;
	float PlanetDetailFadeEnd;
	float PlanetTime;
	float PlanetOceanOpacity;

	//<--- Ocean ---<<
	float4 PlanetOceanShallowColor;
	float4 PlanetOceanDeepColor;

	float PlanetOceanDepthFalloff;
	float PlanetOceanRoughness;
	float PlanetWaveStrength;
	float PlanetWaveScale;

	float PlanetWaveSpeed;
	float PlanetOceanFresnel;
	float2 PlanetOceanPadding;

	// The planet's transform and its inverse. A chunk's vertices are measured from the chunk's own
	// middle, and ground textures are placed by position in the planet's own space, so the shader has
	// to get from one to the other.
	//
	// It used to do that from a per-chunk constant buffer holding the chunk's origin, which meant a
	// buffer update between every draw -- two per chunk with the model matrix, several hundred per
	// frame, each one a rename the driver has to chase. Going through world space instead needs
	// nothing per chunk: the origin is already in the model matrix as its translation.
	row_major matrix PlanetToWorld;
	row_major matrix WorldToPlanet;

	PlanetBiomeData PlanetBiomes[PLANET_MAX_BIOMES];
};

Texture2DArray planetAlbedoArray : register(T_PLANET_ALBEDO);
Texture2DArray planetNormalArray : register(T_PLANET_NORMAL);
Texture2DArray planetRoughnessArray : register(T_PLANET_ROUGHNESS);
SamplerState planetGroundSampler : register(S_PLANET_GROUND);

//<--- Mirror of the Window() helper in PlanetBiome.cpp ---<<
float PlanetWindow(float value, float minimum, float maximum, float blend)
{
	if (blend <= 0.0f)
		return value >= minimum && value <= maximum ? 1.0f : 0.0f;

	float rise = smoothstep(minimum, minimum + blend, value);
	float fall = 1.0f - smoothstep(maximum - blend, maximum, value);

	return rise * fall;
}

// Mirror of PlanetBiome::GetWeight, without the early exits the CPU side has.
//
// Leaving them out is deliberate and was measured: an early return inside this function, called from a
// loop whose length comes from a constant buffer, makes fxc structurize the loop body into nested
// branches. Neighbouring pixels take different branches, so the wave executes all of them anyway and
// nothing is saved -- and the branch overhead and register pressure cost more than the four windows
// did. It doubled the cost of the surface pass. Straight-line multiplication is faster here even
// though it does strictly more arithmetic.
float PlanetBiomeWeight(PlanetBiomeData biome, float temperature, float humidity, float height, float slope)
{
	float weight = PlanetWindow(temperature, biome.ClimateRange.x, biome.ClimateRange.y, biome.Blends.x);
	weight *= PlanetWindow(humidity, biome.ClimateRange.z, biome.ClimateRange.w, biome.Blends.y);
	weight *= PlanetWindow(height, biome.HeightSlope.x, biome.HeightSlope.y, biome.Blends.z);
	weight *= PlanetWindow(slope, biome.HeightSlope.z, biome.HeightSlope.w, biome.Blends.w);

	return weight * max(0.0f, biome.Tint.w);
}

struct PlanetBiomeBlendResult
{
	int Indices[PLANET_MAX_LAYERS];
	float Weights[PLANET_MAX_LAYERS];
	int Count;
};

// Every biome's membership, as sixteen weights. The expensive half of the biome rule: sixteen biomes
// times four smoothstep windows, each reading its bounds from the constant buffer.
//
// This belongs in the vertex shader and it is worth being explicit about why. It is a function of the
// climate and nothing else, and the climate arrives interpolated from the vertices -- so evaluating it
// per pixel is evaluating a slowly varying function at the highest rate the hardware offers. Measured
// at 2.1ms of a 4.7ms surface pass. The LOD system already holds triangles near a few pixels across
// (see PlanetLodSettings::TargetTrianglePixels), which puts roughly an order of magnitude fewer vertex
// invocations than pixels behind the same ground, and makes the interpolation error sub-triangle.
//
// The loop is over the fixed maximum rather than PlanetBiomeCount so that it unrolls: the count comes
// from a constant buffer, and a dynamic bound leaves every PlanetBiomes[index] a dynamically indexed
// constant buffer read. Unrolled, all sixteen are constant offsets. Biomes past the count weigh zero
// and fall out of the blend on their own.
// Written out rather than looped into a local array, for the reason the file keeps running into: a
// local array that anything indexes becomes an indexable temporary, and those are allocated for the
// whole shader whether or not the path that touches them runs. The macro takes a literal, so every
// PlanetBiomes read below is a constant offset into the constant buffer.
#define PLANET_WEIGHT_AT(biomeIndex) ((biomeIndex) < count \
	? PlanetBiomeWeight(PlanetBiomes[biomeIndex], temperature, humidity, height, slope) \
	: 0.0f)

void PlanetComputeWeights(float temperature, float humidity, float height, float slope,
                          out float4 weights0, out float4 weights1,
                          out float4 weights2, out float4 weights3)
{
	int count = min(PlanetBiomeCount, PLANET_MAX_BIOMES);

	weights0 = float4(PLANET_WEIGHT_AT(0), PLANET_WEIGHT_AT(1), PLANET_WEIGHT_AT(2), PLANET_WEIGHT_AT(3));
	weights1 = float4(PLANET_WEIGHT_AT(4), PLANET_WEIGHT_AT(5), PLANET_WEIGHT_AT(6), PLANET_WEIGHT_AT(7));
	weights2 = float4(PLANET_WEIGHT_AT(8), PLANET_WEIGHT_AT(9), PLANET_WEIGHT_AT(10), PLANET_WEIGHT_AT(11));
	weights3 = float4(PLANET_WEIGHT_AT(12), PLANET_WEIGHT_AT(13), PLANET_WEIGHT_AT(14), PLANET_WEIGHT_AT(15));
}

// Everything about a biome that is not a texture -- its tint, its roughness, its metalness -- blended
// by weight. This exists because of one property: it is *linear* in the weights, and the weights are a
// smooth function of a climate that arrives interpolated. A linear function of something that varies
// smoothly across a triangle can be evaluated at the three corners and interpolated, and comes out the
// same. So this belongs in the vertex shader, and a planet with no ground textures never touches a
// biome in the pixel shader at all.
//
// Measured, because the obvious suspect was the wrong one. Computing the weights per pixel is not what
// cost -- it was the blend that follows, which reads PlanetBiomes[blend.Indices[slot]] four times over,
// a dynamic index into a 1536 byte constant buffer array. Two milliseconds of a four and a half
// millisecond pass. Summed over all sixteen with literal indices there is no dynamic index anywhere,
// no sort, and no top four to pick.
//
// Using all sixteen rather than the strongest four is a deliberate, tiny divergence from
// PlanetBiomeTable::Classify. It can only differ where five or more biomes overlap at one point, and
// then only by including the ones the CPU drops -- which is the smoother answer, not the wronger one.
// The CPU rule stays authoritative for queries that need a single answer, which is what it is for.
void PlanetBlendFlat(float temperature, float humidity, float height, float slope,
                     out float3 albedo, out float roughness, out float metallic)
{
	int count = min(PlanetBiomeCount, PLANET_MAX_BIOMES);

	float3 tint = 0.0f;
	float roughnessSum = 0.0f;
	float metallicSum = 0.0f;
	float total = 0.0f;

	#define PLANET_ACCUMULATE(biomeIndex) \
		{ \
			float weight = (biomeIndex) < count \
				? PlanetBiomeWeight(PlanetBiomes[biomeIndex], temperature, humidity, height, slope) \
				: 0.0f; \
			tint += PlanetBiomes[biomeIndex].Tint.rgb * weight; \
			roughnessSum += PlanetBiomes[biomeIndex].Surface.x * weight; \
			metallicSum += PlanetBiomes[biomeIndex].Surface.y * weight; \
			total += weight; \
		}

	PLANET_ACCUMULATE(0)  PLANET_ACCUMULATE(1)  PLANET_ACCUMULATE(2)  PLANET_ACCUMULATE(3)
	PLANET_ACCUMULATE(4)  PLANET_ACCUMULATE(5)  PLANET_ACCUMULATE(6)  PLANET_ACCUMULATE(7)
	PLANET_ACCUMULATE(8)  PLANET_ACCUMULATE(9)  PLANET_ACCUMULATE(10) PLANET_ACCUMULATE(11)
	PLANET_ACCUMULATE(12) PLANET_ACCUMULATE(13) PLANET_ACCUMULATE(14) PLANET_ACCUMULATE(15)

	#undef PLANET_ACCUMULATE

	//<--- Nothing matched: show the first biome rather than a hole in the ground ---<<
	if (total <= 0.0f)
	{
		albedo = PlanetBiomes[0].Tint.rgb;
		roughness = PlanetBiomes[0].Surface.x;
		metallic = PlanetBiomes[0].Surface.y;

		return;
	}

	float normalize = 1.0f / total;

	albedo = tint * normalize;
	roughness = roughnessSum * normalize;
	metallic = metallicSum * normalize;
}

#undef PLANET_WEIGHT_AT

// Mirror of PlanetBiomeTable::Classify: the strongest four matches, normalized. Takes the weights
// already computed rather than the climate, so the same selection runs per pixel over interpolated
// numbers -- which is where it has to be, because the blend it feeds samples textures, and a texture
// lookup is not linear in position the way a tint is.
//
// The insertion is written out as four scalars rather than as a loop over an array, which is not a
// style choice: a dynamically indexed write into a local array leaves fxc trying to keep it in
// indexable temporaries, and those cost occupancy for the whole shader. Four named slots have no
// array to index and compile to the compares they are.
// One candidate against the running top four. Guarded on the fourth slot first, which rejects most
// biomes in one compare -- and since the slots start at zero, that guard also throws out the weights
// that are zero, including the hair below zero that interpolation can produce between two vertices
// which both had none.
//
// Strictly greater throughout, so equal weights keep the earlier biome, as on the CPU side.
#define PLANET_INSERT_BIOME(candidateWeight, candidateIndex) \
	{ \
		float weight = candidateWeight; \
		if (weight > weight3) \
		{ \
			if (weight > weight0) \
			{ \
				weight3 = weight2; index3 = index2; \
				weight2 = weight1; index2 = index1; \
				weight1 = weight0; index1 = index0; \
				weight0 = weight;  index0 = candidateIndex; \
			} \
			else if (weight > weight1) \
			{ \
				weight3 = weight2; index3 = index2; \
				weight2 = weight1; index2 = index1; \
				weight1 = weight;  index1 = candidateIndex; \
			} \
			else if (weight > weight2) \
			{ \
				weight3 = weight2; index3 = index2; \
				weight2 = weight;  index2 = candidateIndex; \
			} \
			else \
			{ \
				weight3 = weight;  index3 = candidateIndex; \
			} \
		} \
	}

PlanetBiomeBlendResult PlanetClassifyWeights(float4 weights0In, float4 weights1In,
                                             float4 weights2In, float4 weights3In)
{
	float weight0 = 0.0f, weight1 = 0.0f, weight2 = 0.0f, weight3 = 0.0f;
	int index0 = 0, index1 = 0, index2 = 0, index3 = 0;

	PLANET_INSERT_BIOME(weights0In.x, 0)
	PLANET_INSERT_BIOME(weights0In.y, 1)
	PLANET_INSERT_BIOME(weights0In.z, 2)
	PLANET_INSERT_BIOME(weights0In.w, 3)
	PLANET_INSERT_BIOME(weights1In.x, 4)
	PLANET_INSERT_BIOME(weights1In.y, 5)
	PLANET_INSERT_BIOME(weights1In.z, 6)
	PLANET_INSERT_BIOME(weights1In.w, 7)
	PLANET_INSERT_BIOME(weights2In.x, 8)
	PLANET_INSERT_BIOME(weights2In.y, 9)
	PLANET_INSERT_BIOME(weights2In.z, 10)
	PLANET_INSERT_BIOME(weights2In.w, 11)
	PLANET_INSERT_BIOME(weights3In.x, 12)
	PLANET_INSERT_BIOME(weights3In.y, 13)
	PLANET_INSERT_BIOME(weights3In.z, 14)
	PLANET_INSERT_BIOME(weights3In.w, 15)

	PlanetBiomeBlendResult result = (PlanetBiomeBlendResult)0;

	float total = weight0 + weight1 + weight2 + weight3;

	if (total <= 0.0f)
	{
		//<--- Nothing matched: show the first biome rather than a hole in the ground ---<<
		result.Weights[0] = 1.0f;
		result.Count = 1;

		return result;
	}

	float normalize = 1.0f / total;

	result.Indices[0] = index0; result.Weights[0] = weight0 * normalize;
	result.Indices[1] = index1; result.Weights[1] = weight1 * normalize;
	result.Indices[2] = index2; result.Weights[2] = weight2 * normalize;
	result.Indices[3] = index3; result.Weights[3] = weight3 * normalize;

	result.Count = (weight0 > 0.0f ? 1 : 0) + (weight1 > 0.0f ? 1 : 0)
		+ (weight2 > 0.0f ? 1 : 0) + (weight3 > 0.0f ? 1 : 0);

	return result;
}

#undef PLANET_INSERT_BIOME

// Weights of the three planar projections. Triplanar rather than a UV mapping because there is no
// mapping of a sphere onto a plane that does not either seam or stretch, and the ground is where both
// would be seen from a metre away. Sharpness pushes the blend towards the dominant axis: too low and
// every surface is a mix of three, too high and the transition between them is a visible line.
float3 PlanetTriplanarWeights(float3 normal, float sharpness)
{
	float3 weights = pow(abs(normal), sharpness);

	return weights / max(1e-5f, weights.x + weights.y + weights.z);
}

float4 PlanetSampleTriplanar(Texture2DArray textures, float3 position, float3 weights, float scale, float slice)
{
	//<--- Each projection drops the axis it looks along ---<<
	float4 x = textures.Sample(planetGroundSampler, float3(position.zy * scale, slice));
	float4 y = textures.Sample(planetGroundSampler, float3(position.xz * scale, slice));
	float4 z = textures.Sample(planetGroundSampler, float3(position.xy * scale, slice));

	return x * weights.x + y * weights.y + z * weights.z;
}

// Whiteout blend of three tangent space normals, which is the standard way to combine triplanar
// normal maps: the projections disagree about which way is up, and averaging their vectors flattens
// the result, while swapping the tangent axes per projection and summing keeps the detail.
float3 PlanetSampleTriplanarNormal(Texture2DArray textures, float3 position, float3 weights, float scale,
                                   float slice, float3 surfaceNormal, float strength)
{
	float3 x = textures.Sample(planetGroundSampler, float3(position.zy * scale, slice)).xyz * 2.0f - 1.0f;
	float3 y = textures.Sample(planetGroundSampler, float3(position.xz * scale, slice)).xyz * 2.0f - 1.0f;
	float3 z = textures.Sample(planetGroundSampler, float3(position.xy * scale, slice)).xyz * 2.0f - 1.0f;

	x.xy *= strength;
	y.xy *= strength;
	z.xy *= strength;

	float3 signs = sign(surfaceNormal);

	float3 normalX = float3(x.z * signs.x, x.y, x.x);
	float3 normalY = float3(y.x, y.z * signs.y, y.y);
	float3 normalZ = float3(z.x, z.y, z.z * signs.z);

	return normalize(normalX * weights.x + normalY * weights.y + normalZ * weights.z);
}

#endif
