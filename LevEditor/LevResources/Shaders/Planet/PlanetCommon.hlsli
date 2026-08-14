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

// Mirror of PlanetBiomeTable::Classify: the strongest four matches, normalized.
//
// The insertion is written out as four scalars rather than as a loop over an array, which is not a
// style choice: the biome count comes from a constant buffer, so the outer loop has to stay dynamic,
// and a dynamically indexed write into a local array inside it leaves fxc trying to unroll a loop
// whose length it does not know. Four named slots have no array to index and compile to the compares
// they are.
PlanetBiomeBlendResult PlanetClassify(float temperature, float humidity, float height, float slope)
{
	float weight0 = 0.0f, weight1 = 0.0f, weight2 = 0.0f, weight3 = 0.0f;
	int index0 = 0, index1 = 0, index2 = 0, index3 = 0;

	int count = min(PlanetBiomeCount, PLANET_MAX_BIOMES);

	for (int index = 0; index < count; ++index)
	{
		float weight = PlanetBiomeWeight(PlanetBiomes[index], temperature, humidity, height, slope);
		if (weight <= 0.0f) continue;

		//<--- Strictly greater, so equal weights keep the earlier biome, as on the CPU side ---<<
		if (weight > weight0)
		{
			weight3 = weight2; index3 = index2;
			weight2 = weight1; index2 = index1;
			weight1 = weight0; index1 = index0;
			weight0 = weight;  index0 = index;
		}
		else if (weight > weight1)
		{
			weight3 = weight2; index3 = index2;
			weight2 = weight1; index2 = index1;
			weight1 = weight;  index1 = index;
		}
		else if (weight > weight2)
		{
			weight3 = weight2; index3 = index2;
			weight2 = weight;  index2 = index;
		}
		else if (weight > weight3)
		{
			weight3 = weight;  index3 = index;
		}
	}

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
