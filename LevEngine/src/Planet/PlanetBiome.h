#pragma once

#include "Kernel/Core.h"
#include "Math/Math.h"
#include "Math/Color.h"

namespace LevEngine
{
	// A biome is a window in climate, height and slope, and the look the ground has inside it.
	//
	// Nothing assigns biomes to places. Every biome states the conditions it lives in and the ground
	// takes whichever ones it qualifies for, blended by how well it qualifies -- so a slope that is
	// half tundra and half bare rock is shaded as half of each, and the boundary between them moves
	// when the climate does. Painting biomes onto a map would need the map regenerated whenever a
	// mountain moved; this way the mountain brings its snow line with it.
	//
	// The same rule runs on the GPU, over the same numbers uploaded to a constant buffer. See
	// PlanetCommon.hlsli.
	struct LEV_API PlanetBiome
	{
		String Name = "Biome";

		//<--- Climate window: degrees Celsius, and rainfall from 0 to 1 ---<<
		float MinTemperature = -80.0f;
		float MaxTemperature = 80.0f;
		float MinHumidity = 0.0f;
		float MaxHumidity = 1.0f;

		//<--- Height window, signed world units either side of sea level ---<<
		float MinHeight = -100000.0f;
		float MaxHeight = 100000.0f;

		// Slope, as one minus the cosine of the angle from straight up: 0 is level ground, 1 is a
		// vertical wall. This is what keeps grass off cliffs -- no climate rule can, because a cliff
		// has the same temperature and rainfall as the meadow above it.
		float MinSlope = 0.0f;
		float MaxSlope = 1.0f;

		// Width of the fade at each edge of the four windows, in the units of that window. Zero
		// gives a hard line, which is visible as a contour on the ground and almost never wanted;
		// the defaults are roughly a tenth of a typical window.
		float TemperatureBlend = 4.0f;
		float HumidityBlend = 0.1f;
		float HeightBlend = 60.0f;
		float SlopeBlend = 0.1f;

		//<--- Look ---<<

		//<--- Multiplied into the albedo texture, so one texture can serve several biomes ---<<
		Color Tint = Color(1.0f, 1.0f, 1.0f, 1.0f);

		float Roughness = 0.9f;
		float Metallic = 0.0f;

		//<--- World units one tile of the texture covers ---<<
		float TextureScale = 12.0f;

		//<--- Slice of the biome set's texture arrays this biome samples ---<<
		int32_t TextureIndex = 0;

		float NormalStrength = 1.0f;

		// Where a biome sits when several fit equally well. A beach and a meadow can both be valid
		// at the top of a shore; the higher priority takes the ground rather than the two averaging
		// into something that is neither.
		float Priority = 1.0f;

		[[nodiscard]] float GetWeight(float temperature, float humidity, float height, float slope) const;
	};

	// The number of biomes a planet can have, and the number that can meet at one point.
	//
	// Sixteen because the whole table is uploaded to a constant buffer and read by every pixel of the
	// planet, and four because that is how many can plausibly meet: a height boundary crossing a
	// climate boundary is four, and anything past that is a table with windows too wide to mean
	// anything. Weights beyond the fourth are dropped and the rest renormalized.
	inline constexpr uint32_t k_MaxPlanetBiomes = 16;
	inline constexpr uint32_t k_MaxBlendedBiomes = 4;

	// Which biomes a point is, and how much of each. Weights sum to 1 unless nothing matched at all,
	// in which case Count is 0 and the caller falls back to the first biome in the set.
	struct LEV_API PlanetBiomeBlend
	{
		uint32_t Count = 0;
		int32_t Indices[k_MaxBlendedBiomes]{};
		float Weights[k_MaxBlendedBiomes]{};
	};

	struct LEV_API PlanetBiomeTable
	{
		// Keeps the strongest k_MaxBlendedBiomes matches and normalizes them. Biomes are weighted by
		// their own Priority before being compared, which is what lets two valid biomes resolve to
		// one instead of averaging.
		[[nodiscard]] static PlanetBiomeBlend Classify(const Vector<PlanetBiome>& biomes,
		                                               float temperature, float humidity,
		                                               float height, float slope);

		// The strongest single match, for anything that wants an answer rather than a blend --
		// footstep sounds, what grows here, what the minimap paints.
		[[nodiscard]] static int32_t ClassifyDominant(const Vector<PlanetBiome>& biomes,
		                                              float temperature, float humidity,
		                                              float height, float slope);

		// A set that covers an Earthlike planet from sea floor to snow line. Used as the default so
		// that a planet added to a scene has ground with something on it before anybody has opened
		// the biome editor.
		//
		// The height windows are written against one vertical scale and scaled to the one asked for
		// here -- PlanetShapeSettings::GetMaxElevation and GetMinElevation. That is not a detail: the
		// windows are absolute distances from sea level, so on a planet with nine units of relief
		// rather than four hundred, every point on land falls inside the beach's window at once and
		// the whole globe comes out a single flat shade of sand. Defaulted to the reference scale, so
		// an authored set still starts from the numbers as written.
		[[nodiscard]] static Vector<PlanetBiome> CreateEarthlikeSet(float maxElevation = 484.0f,
		                                                            float minElevation = -234.0f);
	};
}
