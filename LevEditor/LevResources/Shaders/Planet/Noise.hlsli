#ifndef LEV_PLANET_NOISE_HLSLI
#define LEV_PLANET_NOISE_HLSLI

// Mirror of LevEngine/src/Math/Noise/Noise.cpp. The two must produce the same number for the same
// input, because the mesh is displaced on the CPU and the surface shader adds detail below the size
// of a vertex on top of it -- if they disagreed, the shading would describe a slope the geometry
// does not have. Every constant, every operation and the order of the operations is the same on
// both sides. Change one, change the other.

static const float LEV_F3 = 1.0f / 3.0f;
static const float LEV_G3 = 1.0f / 6.0f;

// Sixteen entries rather than twelve, the last four repeats, so the index is a mask of the hash
// instead of a remainder. Integer division is one of the few things a GPU is genuinely bad at, and
// this runs several times per noise sample. See the C++ side.
static const float3 LEV_GRADIENTS[16] =
{
	float3(1, 1, 0), float3(-1, 1, 0), float3(1, -1, 0), float3(-1, -1, 0),
	float3(1, 0, 1), float3(-1, 0, 1), float3(1, 0, -1), float3(-1, 0, -1),
	float3(0, 1, 1), float3(0, -1, 1), float3(0, 1, -1), float3(0, -1, -1),
	float3(1, 1, 0), float3(0, -1, 1), float3(-1, 1, 0), float3(0, -1, -1),
};

uint NoiseHash(int3 cell, uint seed)
{
	uint h = seed + 0x9E3779B9u;

	h ^= asuint(cell.x) * 0x85EBCA6Bu;
	h *= 0xC2B2AE35u;
	h ^= h >> 15;

	h ^= asuint(cell.y) * 0x27D4EB2Fu;
	h *= 0x165667B1u;
	h ^= h >> 13;

	h ^= asuint(cell.z) * 0x9E3779B1u;
	h *= 0x85EBCA77u;
	h ^= h >> 16;

	return h;
}

float NoiseHashToFloat(uint hash)
{
	return float(hash >> 8) * (1.0f / 16777216.0f);
}

float NoiseCornerContribution(float3 offset, int3 cell, uint seed)
{
	float t = 0.6f - dot(offset, offset);
	if (t < 0.0f) return 0.0f;

	float3 g = LEV_GRADIENTS[NoiseHash(cell, seed) & 15u];

	t *= t;
	return t * t * dot(g, offset);
}

// Three dimensional simplex noise, roughly [-1, 1]. See the header of the C++ side for why it is
// simplex and why it is three dimensional.
float NoiseSimplex(float3 position, uint seed)
{
	float s = (position.x + position.y + position.z) * LEV_F3;
	int3 cell = int3(floor(position + s));

	float t = float(cell.x + cell.y + cell.z) * LEV_G3;
	float3 offset0 = position - (float3(cell) - t);

	//<--- Which of the six tetrahedra of the cell we are in, as the order the coordinates sort in ---<<
	int3 step1;
	int3 step2;

	if (offset0.x >= offset0.y)
	{
		if (offset0.y >= offset0.z)
		{
			step1 = int3(1, 0, 0);
			step2 = int3(1, 1, 0);
		}
		else if (offset0.x >= offset0.z)
		{
			step1 = int3(1, 0, 0);
			step2 = int3(1, 0, 1);
		}
		else
		{
			step1 = int3(0, 0, 1);
			step2 = int3(1, 0, 1);
		}
	}
	else
	{
		if (offset0.y < offset0.z)
		{
			step1 = int3(0, 0, 1);
			step2 = int3(0, 1, 1);
		}
		else if (offset0.x < offset0.z)
		{
			step1 = int3(0, 1, 0);
			step2 = int3(0, 1, 1);
		}
		else
		{
			step1 = int3(0, 1, 0);
			step2 = int3(1, 1, 0);
		}
	}

	float3 offset1 = offset0 - float3(step1) + LEV_G3;
	float3 offset2 = offset0 - float3(step2) + 2.0f * LEV_G3;
	float3 offset3 = offset0 - 1.0f + 3.0f * LEV_G3;

	float n0 = NoiseCornerContribution(offset0, cell, seed);
	float n1 = NoiseCornerContribution(offset1, cell + step1, seed);
	float n2 = NoiseCornerContribution(offset2, cell + step2, seed);
	float n3 = NoiseCornerContribution(offset3, cell + int3(1, 1, 1), seed);

	return 32.0f * (n0 + n1 + n2 + n3);
}

// Simplex noise and its analytic gradient, from one evaluation.
//
// Anything that bends a normal by a noise field needs the field's slope, and the obvious way to get it
// is to sample two or three times and subtract. That costs two or three times as much as it needs to:
// the derivative of this noise is available in closed form from the same four corner contributions the
// value is built from, because each corner is a falloff times a linear ramp and both differentiate by
// hand. One evaluation instead of three, and the slope is exact rather than a difference over some
// arbitrary offset.
//
// d/dp [ t^4 * dot(g, o) ] = t^4 * g - 8 * t^3 * dot(g, o) * o, with t = 0.6 - dot(o, o).
//
// Written out corner by corner with no local arrays, which matters more than it looks: an array
// indexed by a loop counter becomes an indexable temp, and those are allocated out of a scratch pool
// that costs occupancy for the whole shader -- even the parts that never touch it, and even when the
// code is branched over at runtime. Putting the four corners in an array cost six milliseconds a frame
// in the surface pass and did not get any of it back when the feature was switched off.
void NoiseCornerGradient(float3 offset, int3 cell, uint seed, inout float value, inout float3 gradient)
{
	float falloff = 0.6f - dot(offset, offset);
	if (falloff <= 0.0f) return;

	float3 g = LEV_GRADIENTS[NoiseHash(cell, seed) & 15u];
	float ramp = dot(g, offset);

	float falloff2 = falloff * falloff;

	value += falloff2 * falloff2 * ramp;
	gradient += falloff2 * falloff2 * g - 8.0f * falloff2 * falloff * ramp * offset;
}

float NoiseSimplexGradient(float3 position, uint seed, out float3 gradient)
{
	float s = (position.x + position.y + position.z) * LEV_F3;
	int3 cell = int3(floor(position + s));

	float t = float(cell.x + cell.y + cell.z) * LEV_G3;
	float3 offset0 = position - (float3(cell) - t);

	int3 step1;
	int3 step2;

	if (offset0.x >= offset0.y)
	{
		if (offset0.y >= offset0.z)
		{
			step1 = int3(1, 0, 0);
			step2 = int3(1, 1, 0);
		}
		else if (offset0.x >= offset0.z)
		{
			step1 = int3(1, 0, 0);
			step2 = int3(1, 0, 1);
		}
		else
		{
			step1 = int3(0, 0, 1);
			step2 = int3(1, 0, 1);
		}
	}
	else
	{
		if (offset0.y < offset0.z)
		{
			step1 = int3(0, 0, 1);
			step2 = int3(0, 1, 1);
		}
		else if (offset0.x < offset0.z)
		{
			step1 = int3(0, 1, 0);
			step2 = int3(0, 1, 1);
		}
		else
		{
			step1 = int3(0, 1, 0);
			step2 = int3(1, 1, 0);
		}
	}

	float3 offset1 = offset0 - float3(step1) + LEV_G3;
	float3 offset2 = offset0 - float3(step2) + 2.0f * LEV_G3;
	float3 offset3 = offset0 - 1.0f + 3.0f * LEV_G3;

	float value = 0.0f;
	gradient = 0.0f;

	NoiseCornerGradient(offset0, cell, seed, value, gradient);
	NoiseCornerGradient(offset1, cell + step1, seed, value, gradient);
	NoiseCornerGradient(offset2, cell + step2, seed, value, gradient);
	NoiseCornerGradient(offset3, cell + int3(1, 1, 1), seed, value, gradient);

	gradient *= 32.0f;

	return 32.0f * value;
}

// The fractal sums. Packed as a float4/uint pair rather than a struct with named fields so a
// constant buffer can hand them over without a layout to keep in step:
// settings = (frequency, lacunarity, gain, octaves).

float NoiseFractal(float3 position, float4 settings, uint seed)
{
	float sum = 0.0f;
	float amplitude = 1.0f;
	float totalAmplitude = 0.0f;
	float3 p = position * settings.x;

	int octaves = int(settings.w);
	for (int octave = 0; octave < octaves; ++octave)
	{
		sum += NoiseSimplex(p, seed + uint(octave) * 0x9E3779B9u) * amplitude;
		totalAmplitude += amplitude;

		p *= settings.y;
		amplitude *= settings.z;
	}

	return totalAmplitude > 0.0f ? sum / totalAmplitude : 0.0f;
}

float NoiseRidged(float3 position, float4 settings, uint seed)
{
	float sum = 0.0f;
	float amplitude = 1.0f;
	float totalAmplitude = 0.0f;
	float weight = 1.0f;
	float3 p = position * settings.x;

	int octaves = int(settings.w);
	for (int octave = 0; octave < octaves; ++octave)
	{
		float n = 1.0f - abs(NoiseSimplex(p, seed + uint(octave) * 0x9E3779B9u));
		n *= n;
		n *= weight;

		weight = clamp(n * 2.0f, 0.0f, 1.0f);

		sum += n * amplitude;
		totalAmplitude += amplitude;

		p *= settings.y;
		amplitude *= settings.z;
	}

	return totalAmplitude > 0.0f ? sum / totalAmplitude : 0.0f;
}

float NoiseBillow(float3 position, float4 settings, uint seed)
{
	float sum = 0.0f;
	float amplitude = 1.0f;
	float totalAmplitude = 0.0f;
	float3 p = position * settings.x;

	int octaves = int(settings.w);
	for (int octave = 0; octave < octaves; ++octave)
	{
		sum += abs(NoiseSimplex(p, seed + uint(octave) * 0x9E3779B9u)) * amplitude;
		totalAmplitude += amplitude;

		p *= settings.y;
		amplitude *= settings.z;
	}

	return totalAmplitude > 0.0f ? sum / totalAmplitude : 0.0f;
}

float3 NoiseWarp(float3 position, float strength, float frequency, uint seed)
{
	float3 p = position * frequency;

	float3 offset = float3(
		NoiseSimplex(p, seed),
		NoiseSimplex(p, seed + 0x68BC21EBu),
		NoiseSimplex(p, seed + 0x02E5BE93u));

	return position + offset * strength;
}

#endif
