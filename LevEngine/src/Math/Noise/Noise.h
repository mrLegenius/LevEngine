#pragma once

#include "Kernel/Core.h"
#include "Math/Math.h"

namespace LevEngine
{
	// Gradient noise and the fractal sums built on it.
	//
	// Everything here has a line by line counterpart in LevResources/Shaders/Planet/Noise.hlsli,
	// and the two have to keep producing the same number for the same input. The planet's shape and
	// climate are evaluated on the CPU -- that is what collision, spawning and height queries read --
	// while the surface shader evaluates the same functions again for detail below the size of a
	// vertex. A shader that disagreed with the mesh would shade a slope that is not there.
	//
	// That is why the hash is integer arithmetic rather than a permutation table: a table is a third
	// thing to keep in step, and uint multiplication and shifts mean exactly the same in HLSL as
	// they do here. Everything else is float arithmetic in the same order.
	namespace Noise
	{
		// Three dimensional noise, which is the only kind that is any use on a sphere: two
		// dimensional noise has to be mapped onto the surface somehow, and every mapping of a plane
		// onto a sphere either seams or stretches. Sampling a solid at points on the surface has
		// neither problem, and is the reason a cube-sphere planet needs no seam handling at all.
		//
		// Simplex rather than Perlin because Perlin's cell grid is visible on a sphere: its
		// gradients vanish on the axis planes, which lays a faint cross over the poles and the
		// equator no matter how many octaves are stacked on it.
		//
		// Returns roughly [-1, 1]. Zero at every lattice point, so a planet whose continent noise
		// is sampled at frequency 1 has coastline through the cube's corners; frequencies below
		// about 1.5 are not worth using for that reason.
		[[nodiscard]] LEV_API float Simplex(Vector3 point, uint32_t seed);

		// The parameters of a fractal sum. One octave doubling in frequency and halving in
		// amplitude is the default because it is what natural terrain roughly does: the same shape
		// at every scale, each scale contributing less.
		struct LEV_API FractalSettings
		{
			//<--- Size of the largest feature: 1 puts one bump across the planet ---<<
			float Frequency = 1.0f;

			int32_t Octaves = 6;

			//<--- Frequency step per octave. 2 is one doubling; irrational values near 2 keep the
			//octaves from lining up their lattices, which is what produces visible grid ---<<
			float Lacunarity = 2.03f;

			//<--- Amplitude step per octave. Below 0.5 the fine detail dies out and the surface
			//reads as smooth hills; above it the terrain gets rough and noisy ---<<
			float Gain = 0.5f;

			uint32_t Seed = 0;

			friend bool operator==(const FractalSettings&, const FractalSettings&) = default;
		};

		// Plain fractal sum -- rolling, symmetric hills. Roughly [-1, 1], normalized by the total
		// amplitude so changing the octave count does not change how tall the result is.
		[[nodiscard]] LEV_API float Fractal(Vector3 point, const FractalSettings& settings);

		// Ridged multifractal: 1 - |noise| per octave, so the zero crossings of the noise become
		// creases. This is what makes mountains rather than hills -- real ranges are made of sharp
		// ridges with broad valleys between them, which a symmetric sum cannot produce at any
		// amplitude. Each octave is also weighted by the one above it, so detail only appears where
		// the larger shape is already high, which keeps the valleys smooth.
		//
		// Returns [0, 1].
		[[nodiscard]] LEV_API float Ridged(Vector3 point, const FractalSettings& settings);

		// |noise| per octave: the inverse of ridged, puffy blobs with creases in the low ground.
		// Used for cloud-like and dune-like fields rather than terrain.
		// Returns [0, 1].
		[[nodiscard]] LEV_API float Billow(Vector3 point, const FractalSettings& settings);

		// Moves the sample point by a vector of noise before sampling. A straight fractal sum is
		// isotropic and its features all look the same; warping the domain first stretches and
		// folds them, which is what gives coastlines their bays and peninsulas instead of round
		// blobs. Strength is in units of the point's own scale, so 0.1 is a tenth of a feature.
		[[nodiscard]] LEV_API Vector3 Warp(Vector3 point, float strength, float frequency, uint32_t seed);

		//<--- Integer hash, and the gradient it selects. Public because the shader side needs the
		//same two functions and they are worth testing directly ---<<
		[[nodiscard]] LEV_API uint32_t Hash(int32_t x, int32_t y, int32_t z, uint32_t seed);

		// A number in [0, 1) from a hash, for anything that needs a stable random value per cell --
		// which tree is at a spot, how weathered a patch of rock is.
		[[nodiscard]] LEV_API float HashToFloat(uint32_t hash);
	}
}
