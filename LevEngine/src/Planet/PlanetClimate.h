#pragma once

#include "Kernel/Core.h"
#include "Math/Math.h"
#include "Math/Noise/Noise.h"

namespace LevEngine
{
	class PlanetShape;
	struct PlanetShapeSample;

	// Temperature and rainfall over the surface.
	//
	// Biomes are not painted on: they are read out of these two numbers, the way the real ones are.
	// That is the whole reason this exists rather than a height-to-colour ramp -- a ramp puts snow on
	// every mountain including the ones on the equator, and desert wherever the artist put desert. A
	// climate puts snow where it is cold and desert where the rain does not reach, so a planet's
	// bands and its rain shadows come out of its shape instead of being drawn onto it.
	//
	// Directions are in the planet's own space, where +Y is the north pole.
	struct LEV_API PlanetClimateSettings
	{
		uint32_t Seed = 7;

		//<--- Temperature, in degrees Celsius at sea level ---<<

		float EquatorTemperature = 30.0f;
		float PoleTemperature = -25.0f;

		// Exponent on cos(latitude). 1 is a straight cosine, which cools off too fast in the
		// tropics; above 1 flattens the tropics and steepens the temperate zone, which is closer to
		// what a planet with an atmosphere moving heat polewards actually does.
		float LatitudeFalloff = 1.4f;

		// Degrees lost per kilometre of altitude. 6.5 is Earth's average, and it is the reason a
		// mountain on the equator is snow-capped: three kilometres of it is twenty degrees.
		float LapseRate = 6.5f;

		// World units to kilometres, for the lapse rate above. This is also the vertical exaggeration
		// knob, and on a planet small enough to walk across it has to be exaggerated: a world four
		// thousand units across has mountains a few hundred units tall, which at a metre per unit is a
		// few hundred metres and nowhere near cold enough to hold snow. Twenty metres per unit puts
		// those same peaks at seven kilometres, and the snow line lands where the geometry looks like
		// it should. Lower it towards 0.001 as the planet grows towards a real one.
		float KilometersPerUnit = 0.02f;

		//<--- Regional variation, so latitude bands are not perfectly straight ---<<
		Noise::FractalSettings TemperatureVariation{3.0f, 3, 2.03f, 0.5f, 0};
		float TemperatureVariationRange = 5.0f;

		//<--- Humidity, 0 (arid) to 1 (saturated) ---<<

		// Rain comes off the sea, so how wet a place is depends mostly on how far from one it is.
		// These two are the ends of that: a coast and the middle of a continent.
		float HumidityAtCoast = 0.85f;
		float HumidityInland = 0.2f;

		// The banded circulation: air rises at the equator and dumps its water, comes down dry
		// around thirty degrees, rises again around sixty. This is why the great deserts sit in two
		// belts rather than being scattered, and why the equator is rainforest. 0 turns the bands
		// off and leaves rainfall to distance and noise alone.
		float CirculationStrength = 0.55f;

		Noise::FractalSettings HumidityVariation{4.0f, 3, 2.03f, 0.5f, 0};
		float HumidityVariationRange = 0.22f;

		// Rain shadow: a range takes the water out of the air crossing it, so its far side is dry.
		// This is the single strongest local effect on rainfall there is -- deserts sit behind
		// mountains far more often than they sit far from the sea.
		float RainShadowStrength = 0.6f;

		//<--- How far upwind to look, as an arc in degrees, so it scales with the planet ---<<
		float RainShadowArc = 4.0f;

		//<--- Upwind relief, in world units, that dries the air completely ---<<
		float RainShadowRelief = 700.0f;

		// Cold air holds less water, so height dries as well as chills. Separate from the rain
		// shadow: this is what makes a high plateau arid even with nothing upwind of it.
		float AltitudeDrying = 0.3f;

		friend bool operator==(const PlanetClimateSettings&, const PlanetClimateSettings&) = default;
	};

	struct PlanetClimateSample
	{
		float Temperature = 0.0f; //<--- Degrees Celsius ---<<
		float Humidity = 0.0f;    //<--- 0..1 ---<<

		//<--- Signed, -90 at the south pole. Kept because biome rules and the editor both read it ---<<
		float LatitudeDegrees = 0.0f;
	};

	class LEV_API PlanetClimate
	{
	public:
		PlanetClimate() = default;
		explicit PlanetClimate(const PlanetClimateSettings& settings) : m_Settings(settings) { }

		[[nodiscard]] const PlanetClimateSettings& GetSettings() const { return m_Settings; }
		void SetSettings(const PlanetClimateSettings& settings) { m_Settings = settings; }

		// The shape is needed for more than the elevation it already handed over: the rain shadow
		// asks how high the ground is upwind, which is another sample of it.
		[[nodiscard]] PlanetClimateSample Sample(const PlanetShape& shape, Vector3 direction,
		                                         const PlanetShapeSample& shapeSample) const;

		// Where the point sits in the banded circulation: 1 in a rising, raining belt (the equator
		// and around sixty degrees), 0 in a descending, dry one (around thirty, and the poles).
		[[nodiscard]] static float GetCirculationWetness(float latitudeDegrees);

		// Direction the prevailing wind blows towards, as a unit vector tangent to the surface.
		// Follows the same belts as the wetness above, because it is the same circulation seen
		// sideways: easterly trades in the tropics, westerlies in the temperate zone, easterlies
		// again at the pole.
		[[nodiscard]] static Vector3 GetPrevailingWind(Vector3 direction, float latitudeDegrees);

	private:
		PlanetClimateSettings m_Settings;
	};
}
