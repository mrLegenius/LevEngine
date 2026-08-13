#pragma once

#include "Kernel/Core.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	// Planets ship as presets because the physical coefficients are not something you can dial in
	// by hand: every one of them is a per-channel extinction rate in 1/km.
	enum class AtmospherePreset
	{
		Earth = 0,
		Mars = 1,
		Titan = 2,
		AlienGreen = 3,
		Custom = 4,
	};

	// The planet an entity's sky is rendered from. One per scene -- the first one found wins.
	// All lengths are kilometres, which is the scale the scattering coefficients are defined in.
	// World space is tied to it through UnitsToKilometers.
	REGISTER_PARSE_TYPE(AtmosphereComponent);
	struct LEV_API AtmosphereComponent
	{
		AtmospherePreset Preset = AtmospherePreset::Earth;

		//<--- Geometry ---<<
		float PlanetRadius = 6360.0f;
		float AtmosphereHeight = 60.0f;

		// One world unit is one metre by default. The camera's height above SeaLevelHeight is
		// what decides where in the atmosphere the view ray starts.
		float UnitsToKilometers = 0.001f;
		float SeaLevelHeight = 0.0f;

		//<--- Rayleigh: air molecules, scatters short wavelengths, makes the sky blue ---<<
		Vector3 RayleighScattering = {5.802e-3f, 13.558e-3f, 33.1e-3f};
		float RayleighScaleHeight = 8.0f;

		//<--- Mie: aerosols and dust, mostly forward scattering, makes the haze around the sun ---<<
		float MieScattering = 3.996e-3f;
		float MieAbsorption = 4.4e-4f;
		float MieScaleHeight = 1.2f;
		float MieAnisotropy = 0.8f;

		//<--- Absorption layer: ozone on Earth. Pure absorption, this is what keeps ---<<
		//<--- the zenith blue instead of grey once the sun is below the horizon      ---<<
		Vector3 AbsorptionCoefficients = {0.650e-3f, 1.881e-3f, 0.085e-3f};
		float AbsorptionLayerCenter = 25.0f;
		float AbsorptionLayerWidth = 15.0f;

		Color GroundColor = Color(0.1f, 0.1f, 0.1f, 1.0f);
		float SkyIntensity = 1.0f;
		bool RenderSunDisks = true;

		//<--- Stars ---<<
		// Hidden by the sky rather than by a switch: a star is seen when it stands out against the
		// background behind it, so the same field that reads as a night sky disappears under a lit one.
		bool RenderStars = true;
		float StarBrightness = 1.0f;

		// Stars turn about the same pole the suns circle -- the planet's spin carries both -- but they
		// are not tied to any sun, because a planet completes a turn against the stars slightly faster
		// than it does against its own sun, which is orbiting it. That difference is one full turn per
		// year, and it is what puts different constellations in the night sky as the year goes by.
		// Days here are the DayLength of the sun the stars keep time with.
		float StarYearLength = 365.25f;

		// Turns of the star field per day. 1 is the honest answer -- the same spin that carries the
		// sun carries the stars -- but a game day of two minutes spins the whole sky at three degrees
		// a second, which reads as a fairground ride rather than a night. Lowering this buys a calm
		// sky at the cost of the stars no longer keeping step with the sun. The physical way to slow
		// them down is a longer DayLength, which slows the sun with them.
		float StarRotationSpeed = 1.0f;

		// Scales how many there are. 1 is around fifteen thousand, a dark rural sky.
		float StarDensity = 1.0f;

		// Scintillation: air moving between the star and the eye. It is strongest near the horizon,
		// where the line of sight crosses the most air, and nothing at all overhead.
		float StarTwinkle = 0.5f;

		// The galaxy seen edge on, as a band of unresolved stars and the dust lanes in front of them.
		float MilkyWayBrightness = 0.5f;

		// The image based lighting chain (cubemap -> irradiance -> prefilter) is far too heavy to
		// rebuild every frame, so it is refreshed when the sun has moved enough to matter, and no
		// more often than this. The sky itself is raymarched per pixel and never steps.
		float SkyLightUpdateInterval = 0.1f;
		float SkyLightUpdateAngle = 1.0f;

		AtmosphereComponent();
		AtmosphereComponent(const AtmosphereComponent&) = default;

		[[nodiscard]] float GetAtmosphereRadius() const { return PlanetRadius + AtmosphereHeight; }
	};

	// A star. Sits on an entity together with a Transform -- the transform's forward direction is
	// the direction the light travels, so the sun is in the opposite direction.
	// Add a DirectionalLightComponent next to it to have it light the scene.
	REGISTER_PARSE_TYPE(SunComponent);
	struct LEV_API SunComponent
	{
		Color Color = LevEngine::Color(1.0f, 1.0f, 1.0f, 1.0f);

		// Radiance of the star at the top of the atmosphere. Drives how bright the sky gets.
		float Intensity = 20.0f;

		// 0.545 degrees is the sun seen from Earth.
		float AngularDiameter = 0.545f;

		// Scene lighting: the sun's colour is multiplied by the atmosphere's transmittance towards
		// it, which is what reddens everything at sunrise and sunset.
		bool DrivesDirectionalLight = true;
		float LightIntensity = 1.0f;

		SunComponent();
		SunComponent(const SunComponent&) = default;
	};

	// A body with no light of its own, which shines by reflecting the scene's stars. This is what
	// keeps a night from being black: it lights the sky, the scene and the light probe exactly like a
	// very dim, very cold star, and shows a phase because only the half turned towards a star is lit.
	// Sits on an entity with a Transform, whose forward direction points away from the body, like
	// SunComponent. Give it a CelestialOrbitComponent offset from the sun's to get a night side.
	REGISTER_PARSE_TYPE(MoonComponent);
	struct LEV_API MoonComponent
	{
		Color Color = LevEngine::Color(0.9f, 0.93f, 1.0f, 1.0f);

		// Portion of a star's radiance the disk sends back. Physically this is around a millionth --
		// rock albedo times the star's solid angle -- which nothing this side of eye adaptation over
		// twenty stops can show. A hundredth is the artistic number: a night that reads as a night
		// but is still lit. Eye adaptation only spans MinExposure..MaxExposure, so going much lower
		// than this really does end up black.
		float Reflectance = 0.01f;

		// 0.52 degrees is the Moon seen from Earth -- close enough to the sun's to eclipse it.
		float AngularDiameter = 0.52f;

		// Exposure of the disk alone, on top of Reflectance. A real moon is a small, very bright thing
		// that delivers very little light: its surface outshines every star in the sky while lighting
		// the ground a hundred thousand times more weakly than the sun. Reflectance sets the light it
		// sends, which the sky, the probe and the directional light all take; this sets how bright the
		// disk itself reads, and nothing else. Without the two apart, a moon dim enough not to turn
		// night into dusk is also a moon dimmer than the stars behind it.
		float DiskBrightness = 20.0f;

		// How much the phase matters. 1 is the real new-to-full swing, 0 is a disk that stays fully
		// lit wherever it is, which is the usual choice when the night has to stay lit.
		float PhaseInfluence = 1.0f;

		bool DrivesDirectionalLight = true;
		float LightIntensity = 1.0f;

		MoonComponent();
		MoonComponent(const MoonComponent&) = default;
	};

	// Moves a celestial body along a circular orbit, which is what produces the day cycle.
	// TimeOfDay 0 puts the body at the horizon on the east side, 0.25 at its highest point.
	REGISTER_PARSE_TYPE(CelestialOrbitComponent);
	struct LEV_API CelestialOrbitComponent
	{
		float DayLength = 120.0f;
		float TimeOfDay = 0.2f;
		bool AutoAdvance = true;

		// Whole days the body has completed. TimeOfDay alone cannot say which day it is, and the star
		// field needs to know: its drift away from the sun is counted in days.
		float DaysElapsed = 0.0f;

		// Tilt of the orbital plane away from straight overhead, and the compass direction the
		// body rises from. Two suns on the same planet usually differ in these.
		float Inclination = 20.0f;
		float NorthOffset = 0.0f;

		CelestialOrbitComponent();
		CelestialOrbitComponent(const CelestialOrbitComponent&) = default;

		// Direction from the planet towards the body. The light travels the opposite way.
		[[nodiscard]] Vector3 GetDirectionToBody() const;

		// Axis this orbit turns about, which for the daily circle of a star is the celestial pole --
		// the point the whole sky, stars included, appears to rotate around.
		[[nodiscard]] Vector3 GetOrbitPole() const;
	};

	// Works out what a moon is receiving from the stars around it. Both the sky constants and the
	// moonlight system need the same answer, and neither owns the other.
	struct LEV_API MoonLighting
	{
		struct Illumination
		{
			// Radiance of the disk with the whole visible side lit, which is what the phase scales.
			Vector3 Radiance = Vector3::Zero;

			// Towards the star that contributes most of that radiance, so the disk knows which way
			// to shade its crescent. Zero when nothing lights the moon at all.
			Vector3 DirectionToStar = Vector3::Zero;

			// Fraction of the visible side that is lit: 1 opposite the star, 0 in front of it.
			float PhaseFactor = 0.0f;
		};

		// Direction is from the planet towards the moon, and is expected to be normalized.
		[[nodiscard]] static Illumination Collect(entt::registry& registry, const MoonComponent& moon,
		                                          const Vector3& directionToMoon);
	};

	struct LEV_API AtmospherePresets
	{
		static void Apply(AtmosphereComponent& component, AtmospherePreset preset);
		static const char* GetName(AtmospherePreset preset);
	};
}
