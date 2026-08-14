#include "levpch.h"
#include "PlanetClimate.h"

#include "PlanetShape.h"

namespace LevEngine
{
	namespace
	{
		constexpr uint32_t k_TemperatureSeed = 0x510E527Fu;
		constexpr uint32_t k_HumiditySeed = 0x9B05688Cu;

		// Width of one cell of the circulation, in degrees of latitude. Three of them reach from the
		// equator to the pole, which is the arrangement Earth has and the reason its deserts are
		// where they are.
		constexpr float k_CirculationCellDegrees = 30.0f;
	}

	float PlanetClimate::GetCirculationWetness(const float latitudeDegrees)
	{
		// One cosine over the three cells: 1 at the equator, 0 at thirty, 1 at sixty, 0 at the pole.
		// The rising limb of a cell rains and its falling limb does not, and they alternate, so a
		// single cosine of the right period is the whole model.
		const float phase = Math::Abs(latitudeDegrees) / k_CirculationCellDegrees * Math::Pi;

		return 0.5f + 0.5f * std::cos(phase);
	}

	Vector3 PlanetClimate::GetPrevailingWind(const Vector3 direction, const float latitudeDegrees)
	{
		// East, as the direction you would walk to follow the planet's spin. Degenerate at the poles,
		// where there is no east; the wind there is returned as zero and the rain shadow skipped.
		Vector3 east = Vector3::UnitY.Cross(direction);

		if (east.LengthSquared() < Math::FloatEpsilon)
			return Vector3::Zero;

		east.Normalize();

		// Surface wind runs the opposite way in neighbouring cells, and the sign flips at the same
		// thirty degree boundaries the wetness does -- both are the same overturning air seen from a
		// different angle. Tropics and poles blow from the east, the temperate zone from the west.
		const float band = std::cos(Math::Abs(latitudeDegrees) / k_CirculationCellDegrees * Math::Pi);

		return band > 0.0f ? -east : east;
	}

	PlanetClimateSample PlanetClimate::Sample(const PlanetShape& shape, const Vector3 direction,
	                                          const PlanetShapeSample& shapeSample) const
	{
		const PlanetClimateSettings& settings = m_Settings;

		PlanetClimateSample sample;

		//<--- Latitude from the pole component of the direction ---<<
		const float sinLatitude = Math::Clamp(direction.y, -1.0f, 1.0f);
		sample.LatitudeDegrees = std::asin(sinLatitude) * Math::RadToDeg;

		const float cosLatitude = Math::Sqrt(Math::Max(0.0f, 1.0f - sinLatitude * sinLatitude));

		//<--- Temperature ---<<

		const float latitudeWeight = Math::Pow(cosLatitude, settings.LatitudeFalloff);
		float temperature = Math::Lerp(settings.PoleTemperature, settings.EquatorTemperature, latitudeWeight);

		// Only height above the sea cools: the sea floor is not twenty degrees warmer for being two
		// kilometres down, and treating it that way would put jungle on the ocean bed.
		const float altitude = Math::Max(0.0f, shapeSample.Elevation);
		const float altitudeKilometers = altitude * settings.KilometersPerUnit;
		temperature -= altitudeKilometers * settings.LapseRate;

		Noise::FractalSettings temperatureNoise = settings.TemperatureVariation;
		temperatureNoise.Seed += settings.Seed + k_TemperatureSeed;
		temperature += Noise::Fractal(direction, temperatureNoise) * settings.TemperatureVariationRange;

		sample.Temperature = temperature;

		//<--- Humidity ---<<

		// Distance from the sea, which is most of the answer, then the circulation band on top of it.
		float humidity = Math::Lerp(settings.HumidityAtCoast, settings.HumidityInland,
		                            shapeSample.Continentality);

		const float wetness = GetCirculationWetness(sample.LatitudeDegrees);
		humidity *= Math::Lerp(1.0f, wetness, settings.CirculationStrength);

		Noise::FractalSettings humidityNoise = settings.HumidityVariation;
		humidityNoise.Seed += settings.Seed + k_HumiditySeed;
		humidity += Noise::Fractal(direction, humidityNoise) * settings.HumidityVariationRange;

		// Rain shadow. One extra elevation sample, taken upwind: if the ground there is higher than
		// here, the air arriving has already been lifted and wrung out.
		if (settings.RainShadowStrength > 0.0f && settings.RainShadowRelief > 0.0f)
		{
			const Vector3 wind = GetPrevailingWind(direction, sample.LatitudeDegrees);

			if (wind.LengthSquared() > 0.0f)
			{
				const float arc = settings.RainShadowArc * Math::DegToRad;

				Vector3 upwind = direction - wind * arc;
				upwind.Normalize();

				const float upwindElevation = shape.GetElevation(upwind);
				const float relief = upwindElevation - shapeSample.Elevation;

				const float shadow = Math::Saturate(relief / settings.RainShadowRelief);
				humidity *= 1.0f - shadow * settings.RainShadowStrength;
			}
		}

		//<--- Cold air carries less water, so altitude dries as well as the shadow behind a range ---<<
		const float dryingByAltitude = Math::Saturate(altitudeKilometers * 0.5f) * settings.AltitudeDrying;
		humidity *= 1.0f - dryingByAltitude;

		// Over water there is no question: the air above a sea is saturated. Blending by the land
		// mask rather than switching keeps the coast continuous, which matters because the beach
		// biome is selected inside exactly that band.
		humidity = Math::Lerp(1.0f, humidity, shapeSample.LandMask);

		sample.Humidity = Math::Saturate(humidity);

		return sample;
	}
}
