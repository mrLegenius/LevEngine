#include "levpch.h"
#include "Noise.h"

namespace LevEngine::Noise
{
	namespace
	{
		//<--- Skew and unskew factors that take the cube lattice to the simplex one in 3D ---<<
		constexpr float k_F3 = 1.0f / 3.0f;
		constexpr float k_G3 = 1.0f / 6.0f;

		// The twelve midpoints of a cube's edges. A gradient set has to be evenly spread over the
		// sphere of directions or the noise shows a bias, and these are the cheapest even set in 3D:
		// every component is 0 or ±1, so the dot product below is two adds and no multiplies.
		//
		// Padded to sixteen with four repeats, which is Perlin's own 2002 fix: the index becomes a mask
		// of the hash rather than a remainder of it, and integer division is one of the few operations
		// a GPU is genuinely bad at -- tens of cycles, several times per noise sample. The repeats bias
		// four directions slightly, which is invisible next to what the division costs.
		constexpr float k_Gradients[16][3] = {
			{1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
			{1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
			{0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1},
			{1, 1, 0}, {0, -1, 1}, {-1, 1, 0}, {0, -1, -1},
		};

		// Contribution of one simplex corner. The falloff is radial and reaches zero before the
		// next corner's does, which is what lets four corners cover the space with no seam.
		float CornerContribution(const float x, const float y, const float z,
		                         const int32_t i, const int32_t j, const int32_t k, const uint32_t seed)
		{
			float t = 0.6f - x * x - y * y - z * z;
			if (t < 0.0f) return 0.0f;

			const uint32_t gradientIndex = Hash(i, j, k, seed) & 15u;
			const float* g = k_Gradients[gradientIndex];

			t *= t;
			return t * t * (g[0] * x + g[1] * y + g[2] * z);
		}
	}

	uint32_t Hash(const int32_t x, const int32_t y, const int32_t z, const uint32_t seed)
	{
		// Multiply-xorshift mixing rather than a permutation table: a table is a fourth thing to
		// keep identical between here and the shader, and the constants below are the same
		// arithmetic in HLSL. The multipliers are the usual odd primes -- what matters is that each
		// coordinate is mixed through the whole word before the next one is folded in, so that
		// stepping one cell in any direction changes every bit of the result.
		uint32_t h = seed + 0x9E3779B9u;

		h ^= static_cast<uint32_t>(x) * 0x85EBCA6Bu;
		h *= 0xC2B2AE35u;
		h ^= h >> 15;

		h ^= static_cast<uint32_t>(y) * 0x27D4EB2Fu;
		h *= 0x165667B1u;
		h ^= h >> 13;

		h ^= static_cast<uint32_t>(z) * 0x9E3779B1u;
		h *= 0x85EBCA77u;
		h ^= h >> 16;

		return h;
	}

	float HashToFloat(const uint32_t hash)
	{
		//<--- Top 24 bits: the low ones of a multiply-xorshift hash are the least mixed ---<<
		return static_cast<float>(hash >> 8) * (1.0f / 16777216.0f);
	}

	float Simplex(const Vector3 point, const uint32_t seed)
	{
		//<--- Skew the sample into the lattice where simplices are unit cells ---<<
		const float s = (point.x + point.y + point.z) * k_F3;
		const auto i = static_cast<int32_t>(Math::Floor(point.x + s));
		const auto j = static_cast<int32_t>(Math::Floor(point.y + s));
		const auto k = static_cast<int32_t>(Math::Floor(point.z + s));

		//<--- and back out, to get the offset from the cell's first corner ---<<
		const float t = static_cast<float>(i + j + k) * k_G3;
		const float x0 = point.x - (static_cast<float>(i) - t);
		const float y0 = point.y - (static_cast<float>(j) - t);
		const float z0 = point.z - (static_cast<float>(k) - t);

		// Which of the six tetrahedra of the cell the point is in, as the order the coordinates
		// sort in. The two middle corners are reached by stepping along the largest coordinate
		// first, then the second largest.
		int32_t i1, j1, k1;
		int32_t i2, j2, k2;

		if (x0 >= y0)
		{
			if (y0 >= z0)
			{
				i1 = 1; j1 = 0; k1 = 0;
				i2 = 1; j2 = 1; k2 = 0;
			}
			else if (x0 >= z0)
			{
				i1 = 1; j1 = 0; k1 = 0;
				i2 = 1; j2 = 0; k2 = 1;
			}
			else
			{
				i1 = 0; j1 = 0; k1 = 1;
				i2 = 1; j2 = 0; k2 = 1;
			}
		}
		else
		{
			if (y0 < z0)
			{
				i1 = 0; j1 = 0; k1 = 1;
				i2 = 0; j2 = 1; k2 = 1;
			}
			else if (x0 < z0)
			{
				i1 = 0; j1 = 1; k1 = 0;
				i2 = 0; j2 = 1; k2 = 1;
			}
			else
			{
				i1 = 0; j1 = 1; k1 = 0;
				i2 = 1; j2 = 1; k2 = 0;
			}
		}

		const float x1 = x0 - static_cast<float>(i1) + k_G3;
		const float y1 = y0 - static_cast<float>(j1) + k_G3;
		const float z1 = z0 - static_cast<float>(k1) + k_G3;

		const float x2 = x0 - static_cast<float>(i2) + 2.0f * k_G3;
		const float y2 = y0 - static_cast<float>(j2) + 2.0f * k_G3;
		const float z2 = z0 - static_cast<float>(k2) + 2.0f * k_G3;

		const float x3 = x0 - 1.0f + 3.0f * k_G3;
		const float y3 = y0 - 1.0f + 3.0f * k_G3;
		const float z3 = z0 - 1.0f + 3.0f * k_G3;

		const float n0 = CornerContribution(x0, y0, z0, i, j, k, seed);
		const float n1 = CornerContribution(x1, y1, z1, i + i1, j + j1, k + k1, seed);
		const float n2 = CornerContribution(x2, y2, z2, i + i2, j + j2, k + k2, seed);
		const float n3 = CornerContribution(x3, y3, z3, i + 1, j + 1, k + 1, seed);

		//<--- 32 brings the sum of four corner falloffs to roughly [-1, 1] ---<<
		return 32.0f * (n0 + n1 + n2 + n3);
	}

	float Fractal(const Vector3 point, const FractalSettings& settings)
	{
		float sum = 0.0f;
		float amplitude = 1.0f;
		float totalAmplitude = 0.0f;
		Vector3 p = point * settings.Frequency;

		for (int32_t octave = 0; octave < settings.Octaves; ++octave)
		{
			// Each octave gets its own seed rather than sampling one field at several scales:
			// otherwise every octave has its zeros on the same lattice points and the sum keeps a
			// faint grid however many are stacked.
			sum += Simplex(p, settings.Seed + static_cast<uint32_t>(octave) * 0x9E3779B9u) * amplitude;
			totalAmplitude += amplitude;

			p *= settings.Lacunarity;
			amplitude *= settings.Gain;
		}

		return totalAmplitude > 0.0f ? sum / totalAmplitude : 0.0f;
	}

	float Ridged(const Vector3 point, const FractalSettings& settings)
	{
		float sum = 0.0f;
		float amplitude = 1.0f;
		float totalAmplitude = 0.0f;

		//<--- Carries how high the previous octave was, so detail only lands on high ground ---<<
		float weight = 1.0f;

		Vector3 p = point * settings.Frequency;

		for (int32_t octave = 0; octave < settings.Octaves; ++octave)
		{
			//<--- Folding the noise about zero turns its zero crossings into creases ---<<
			float n = 1.0f - Math::Abs(Simplex(p, settings.Seed + static_cast<uint32_t>(octave) * 0x9E3779B9u));

			//<--- Squaring sharpens the crease into a ridge instead of a fold ---<<
			n *= n;
			n *= weight;

			// A ridge on a slope is a spur of that slope; a ridge in a valley is noise. Weighting
			// the next octave by this one is what gives a range its branching look, and what keeps
			// valley floors smooth enough to walk.
			weight = Math::Clamp(n * 2.0f, 0.0f, 1.0f);

			sum += n * amplitude;
			totalAmplitude += amplitude;

			p *= settings.Lacunarity;
			amplitude *= settings.Gain;
		}

		return totalAmplitude > 0.0f ? sum / totalAmplitude : 0.0f;
	}

	float Billow(const Vector3 point, const FractalSettings& settings)
	{
		float sum = 0.0f;
		float amplitude = 1.0f;
		float totalAmplitude = 0.0f;
		Vector3 p = point * settings.Frequency;

		for (int32_t octave = 0; octave < settings.Octaves; ++octave)
		{
			sum += Math::Abs(Simplex(p, settings.Seed + static_cast<uint32_t>(octave) * 0x9E3779B9u)) * amplitude;
			totalAmplitude += amplitude;

			p *= settings.Lacunarity;
			amplitude *= settings.Gain;
		}

		return totalAmplitude > 0.0f ? sum / totalAmplitude : 0.0f;
	}

	Vector3 Warp(const Vector3 point, const float strength, const float frequency, const uint32_t seed)
	{
		const Vector3 p = point * frequency;

		// Three fields with unrelated seeds. Offsetting one field instead would give a warp whose
		// components are correlated, which shears the domain in one direction rather than folding it.
		const Vector3 offset
		{
			Simplex(p, seed),
			Simplex(p, seed + 0x68BC21EBu),
			Simplex(p, seed + 0x02E5BE93u),
		};

		return point + offset * strength;
	}
}
