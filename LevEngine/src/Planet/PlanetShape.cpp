#include "levpch.h"
#include "PlanetShape.h"

namespace LevEngine
{
	namespace
	{
		// Each field is offset from the planet's one seed rather than seeded separately, so that
		// changing the seed changes the whole planet and nothing else has to be touched. The
		// per-field Seed in FractalSettings stays an offset on top, for the case where somebody
		// wants the same continents with different mountains.
		constexpr uint32_t k_ContinentSeed = 0x00000000u;
		constexpr uint32_t k_WarpSeed = 0x3C6EF372u;
		constexpr uint32_t k_MountainSeed = 0x6A09E667u;
		constexpr uint32_t k_BeltSeed = 0xBB67AE85u;
		constexpr uint32_t k_DetailSeed = 0xA54FF53Au;

		// How much of the continent field lies between the shore and the deepest sea floor. Wider
		// than ShoreWidth by a good margin: the drop from a beach to the abyss is a longer journey
		// than the one from the beach to dry land, and a narrow basin gives every ocean a trench
		// hugging its coast.
		constexpr float k_OceanBasinWidth = 0.7f;

		// The continent field this far above the shoreline counts as the middle of a landmass. Wide
		// on purpose: it is read as distance from the sea, and a narrow window would call the whole
		// interior of a continent equally inland, leaving nowhere for a rainfall gradient to happen.
		constexpr float k_ContinentalityWidth = 0.55f;

		//<--- Octaves of the continent field that describe the mass rather than its coastline ---<<
		constexpr int32_t k_ContinentalityOctaves = 2;

		Noise::FractalSettings WithSeed(Noise::FractalSettings settings, const uint32_t planetSeed,
		                               const uint32_t fieldSeed)
		{
			settings.Seed += planetSeed + fieldSeed;
			return settings;
		}
	}

	PlanetShapeSample PlanetShape::Sample(const Vector3 direction) const
	{
		const PlanetShapeSettings& settings = m_Settings;

		PlanetShapeSample sample;

		// Warp first, then sample: the continent field is read through a folded domain, so its level
		// sets -- the coastlines -- come out folded with it.
		const Vector3 warped = Noise::Warp(direction, settings.ContinentWarp, settings.ContinentWarpFrequency,
		                                   settings.Seed + k_WarpSeed);

		const Noise::FractalSettings continentSettings = WithSeed(settings.Continents, settings.Seed, k_ContinentSeed);
		sample.Continent = Noise::Fractal(warped, continentSettings);

		sample.LandMask = Math::Smoothstep(settings.SeaLevelThreshold,
		                                  settings.SeaLevelThreshold + settings.ShoreWidth,
		                                  sample.Continent);

		// The same field with only its first octaves is the shape of the landmass without its
		// coastline detail, which is what "how far from the sea am I" actually asks about. A bay
		// cut into a coast should not make the interior behind it wet.
		Noise::FractalSettings broadSettings = continentSettings;
		broadSettings.Octaves = Math::Min(k_ContinentalityOctaves, continentSettings.Octaves);
		const float broadContinent = Noise::Fractal(warped, broadSettings);

		sample.Continentality = Math::Smoothstep(settings.SeaLevelThreshold,
		                                         settings.SeaLevelThreshold + k_ContinentalityWidth,
		                                         broadContinent);

		// Sea floor and dry land are both derived from the same field and simply added: each is zero
		// where the other takes over, so the shoreline needs no special case and cannot crack.
		const float oceanFactor = Math::Smoothstep(settings.SeaLevelThreshold,
		                                           settings.SeaLevelThreshold - k_OceanBasinWidth,
		                                           sample.Continent);

		float elevation = settings.LandHeight * sample.LandMask - settings.OceanDepth * oceanFactor;

		// Belts, so that ranges run in lines across a continent instead of covering it. Folding a low
		// frequency field about its zeros gives lines; thresholding near the fold gives lines of a
		// controlled width.
		const float beltField = 1.0f - Math::Abs(
			Noise::Fractal(direction, WithSeed(settings.MountainBelts, settings.Seed, k_BeltSeed)));
		const float beltMask = Math::Smoothstep(1.0f - settings.MountainBeltWidth, 1.0f, beltField);

		const float ridges = Noise::Ridged(direction, WithSeed(settings.Mountains, settings.Seed, k_MountainSeed));

		//<--- Ranges belong inland; how strictly is the caller's choice ---<<
		const float inland = Math::Lerp(1.0f, sample.Continentality, settings.MountainInlandBias);

		const float mountainFactor = ridges * beltMask * sample.LandMask * inland;
		elevation += settings.MountainHeight * mountainFactor;

		const float detail = Noise::Fractal(direction, WithSeed(settings.Detail, settings.Seed, k_DetailSeed));
		const float reliefWeight = Math::Lerp(1.0f, mountainFactor, settings.DetailReliefBias);
		elevation += settings.DetailHeight * detail * reliefWeight;

		sample.Elevation = elevation;

		return sample;
	}

	Vector3 PlanetShape::GetSurfaceNormal(const Vector3 direction, const float spacing) const
	{
		// Two directions perpendicular to the one we are on. Which two does not matter -- the cross
		// product of the surface offsets is the same normal either way -- only that they are not
		// parallel to it, hence picking the axis the direction is least aligned with.
		const Vector3 up = Math::Abs(direction.y) < 0.9f ? Vector3::UnitY : Vector3::UnitX;

		Vector3 east = up.Cross(direction);
		east.Normalize();

		Vector3 north = direction.Cross(east);
		north.Normalize();

		//<--- Turn the world space spacing into the angle it subtends at this radius ---<<
		const float angle = spacing / Math::Max(m_Settings.Radius, 1.0f);

		auto offsetPoint = [this, direction, angle](const Vector3 tangent, const float sign)
		{
			Vector3 offsetDirection = direction + tangent * (angle * sign);
			offsetDirection.Normalize();

			return GetSurfacePoint(offsetDirection);
		};

		const Vector3 eastDelta = offsetPoint(east, 1.0f) - offsetPoint(east, -1.0f);
		const Vector3 northDelta = offsetPoint(north, 1.0f) - offsetPoint(north, -1.0f);

		Vector3 normal = eastDelta.Cross(northDelta);
		normal.Normalize();

		//<--- Cross order can flip depending on the handedness of the tangents we picked ---<<
		if (normal.Dot(direction) < 0.0f)
			normal = -normal;

		return normal;
	}
}
