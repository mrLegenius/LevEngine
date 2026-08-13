#include "levpch.h"
#include "Atmosphere.h"

#include "../ComponentSerializer.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
	AtmosphereComponent::AtmosphereComponent() = default;
	SunComponent::SunComponent() = default;
	MoonComponent::MoonComponent() = default;
	CelestialOrbitComponent::CelestialOrbitComponent() = default;

	MoonLighting::Illumination MoonLighting::Collect(entt::registry& registry, const MoonComponent& moon,
	                                                 const Vector3& directionToMoon)
	{
		Illumination result;

		const Vector3 albedo = static_cast<Vector3>(moon.Color) * Math::Max(moon.Reflectance, 0.0f);
		const float influence = Math::Clamp(moon.PhaseInfluence, 0.0f, 1.0f);

		float totalWeight = 0.0f;
		float phaseSum = 0.0f;
		float brightestWeight = 0.0f;

		const auto stars = registry.view<Transform, SunComponent>();
		for (const auto entity : stars)
		{
			auto [transform, sun] = stars.get<Transform, SunComponent>(entity);

			const Vector3 directionToStar = -transform.GetForwardDirection();

			// The star's radiance at the top of the atmosphere, not what reaches the ground: a moon
			// sits above the air, so it keeps shining while the star it reflects is below the horizon.
			const Vector3 radiance = static_cast<Vector3>(sun.Color) * Math::Max(sun.Intensity, 0.0f);
			const Vector3 reflected = radiance * albedo;

			// The phase angle is measured at the moon, and for a distant star that is the angle
			// between the star and the planet as seen from there -- so a moon opposite the star is full.
			const float illuminated = (1.0f - directionToMoon.Dot(directionToStar)) * 0.5f;
			const float phase = Math::Lerp(1.0f, Math::Clamp(illuminated, 0.0f, 1.0f), influence);

			const float weight = reflected.x + reflected.y + reflected.z;
			if (weight <= 0.0f) continue;

			result.Radiance += reflected;

			totalWeight += weight;
			phaseSum += phase * weight;

			if (weight > brightestWeight)
			{
				brightestWeight = weight;
				result.DirectionToStar = directionToStar;
			}
		}

		// One phase for a body lit by several stars: whichever of them is putting the most light on
		// it decides how full it looks.
		if (totalWeight > 0.0f)
			result.PhaseFactor = phaseSum / totalWeight;

		return result;
	}

	namespace
	{
		Matrix GetOrbitToWorld(const CelestialOrbitComponent& orbit)
		{
			// The orbit is a unit circle in the XY plane, tilted away from the zenith and then turned
			// to face the chosen compass direction.
			return Matrix::CreateRotationX(orbit.Inclination * Math::DegToRad)
				* Matrix::CreateRotationY(orbit.NorthOffset * Math::DegToRad);
		}
	}

	Vector3 CelestialOrbitComponent::GetDirectionToBody() const
	{
		const float angle = TimeOfDay * Math::Pi2;

		// The body travels a circle that starts at the horizon and peaks overhead, then the whole
		// circle is tilted away from the zenith and turned to face the chosen compass direction.
		const Vector3 onOrbit{std::cos(angle), std::sin(angle), 0.0f};

		Vector3 direction = Vector3::Transform(onOrbit, GetOrbitToWorld(*this));
		direction.Normalize();

		return direction;
	}

	Vector3 CelestialOrbitComponent::GetOrbitPole() const
	{
		// Normal of that same circle. Turning the sky about it by TimeOfDay * 2pi moves anything
		// attached to it exactly the way this orbit moves its body.
		Vector3 pole = Vector3::Transform(Vector3::Backward, GetOrbitToWorld(*this));
		pole.Normalize();

		return pole;
	}

	void AtmospherePresets::Apply(AtmosphereComponent& component, const AtmospherePreset preset)
	{
		switch (preset)
		{
		case AtmospherePreset::Earth:
			component.PlanetRadius = 6360.0f;
			component.AtmosphereHeight = 60.0f;
			component.RayleighScattering = {5.802e-3f, 13.558e-3f, 33.1e-3f};
			component.RayleighScaleHeight = 8.0f;
			component.MieScattering = 3.996e-3f;
			component.MieAbsorption = 4.4e-4f;
			component.MieScaleHeight = 1.2f;
			component.MieAnisotropy = 0.8f;
			component.AbsorptionCoefficients = {0.650e-3f, 1.881e-3f, 0.085e-3f};
			component.AbsorptionLayerCenter = 25.0f;
			component.AbsorptionLayerWidth = 15.0f;
			component.GroundColor = Color(0.1f, 0.1f, 0.1f, 1.0f);
			break;

		// Thin CO2 air with suspended dust: molecular scattering is weak and reddish, and the dust
		// dominates. That is what gives Mars a butterscotch day sky and a blue glow around the
		// setting sun -- the opposite of Earth.
		case AtmospherePreset::Mars:
			component.PlanetRadius = 3390.0f;
			component.AtmosphereHeight = 80.0f;
			component.RayleighScattering = {2.8e-3f, 1.6e-3f, 0.9e-3f};
			component.RayleighScaleHeight = 11.1f;
			component.MieScattering = 8.0e-3f;
			component.MieAbsorption = 2.0e-3f;
			component.MieScaleHeight = 8.0f;
			component.MieAnisotropy = 0.72f;
			component.AbsorptionCoefficients = Vector3::Zero;
			component.AbsorptionLayerCenter = 25.0f;
			component.AbsorptionLayerWidth = 15.0f;
			component.GroundColor = Color(0.19f, 0.09f, 0.05f, 1.0f);
			break;

		// A thick orange haze: heavy forward scattering aerosol over the whole column, so the sun
		// is a diffuse glow rather than a disk.
		case AtmospherePreset::Titan:
			component.PlanetRadius = 2575.0f;
			component.AtmosphereHeight = 200.0f;
			component.RayleighScattering = {6.0e-3f, 8.0e-3f, 14.0e-3f};
			component.RayleighScaleHeight = 21.0f;
			component.MieScattering = 22.0e-3f;
			component.MieAbsorption = 6.0e-3f;
			component.MieScaleHeight = 30.0f;
			component.MieAnisotropy = 0.6f;
			component.AbsorptionCoefficients = {0.4e-3f, 1.6e-3f, 6.0e-3f};
			component.AbsorptionLayerCenter = 60.0f;
			component.AbsorptionLayerWidth = 40.0f;
			component.GroundColor = Color(0.15f, 0.11f, 0.06f, 1.0f);
			break;

		// Not a real place. Scattering peaks in the green channel, so the day sky is green and
		// sunsets go magenta.
		case AtmospherePreset::AlienGreen:
			component.PlanetRadius = 4200.0f;
			component.AtmosphereHeight = 90.0f;
			component.RayleighScattering = {9.0e-3f, 33.0e-3f, 12.0e-3f};
			component.RayleighScaleHeight = 10.0f;
			component.MieScattering = 5.0e-3f;
			component.MieAbsorption = 6.0e-4f;
			component.MieScaleHeight = 2.0f;
			component.MieAnisotropy = 0.75f;
			component.AbsorptionCoefficients = {1.2e-3f, 0.2e-3f, 1.6e-3f};
			component.AbsorptionLayerCenter = 30.0f;
			component.AbsorptionLayerWidth = 20.0f;
			component.GroundColor = Color(0.08f, 0.12f, 0.07f, 1.0f);
			break;

		case AtmospherePreset::Custom:
			break;
		}

		component.Preset = preset;
	}

	const char* AtmospherePresets::GetName(const AtmospherePreset preset)
	{
		switch (preset)
		{
		case AtmospherePreset::Earth: return "Earth";
		case AtmospherePreset::Mars: return "Mars";
		case AtmospherePreset::Titan: return "Titan";
		case AtmospherePreset::AlienGreen: return "Alien (Green)";
		case AtmospherePreset::Custom: return "Custom";
		}

		return "Custom";
	}

	class AtmosphereComponentSerializer final : public ComponentSerializer<AtmosphereComponent, AtmosphereComponentSerializer>
	{
	protected:
		const char* GetKey() override { return "Atmosphere"; }

		void SerializeData(YAML::Emitter& out, const AtmosphereComponent& component) override
		{
			out << YAML::Key << "Preset" << YAML::Value << static_cast<int>(component.Preset);

			out << YAML::Key << "PlanetRadius" << YAML::Value << component.PlanetRadius;
			out << YAML::Key << "AtmosphereHeight" << YAML::Value << component.AtmosphereHeight;
			out << YAML::Key << "UnitsToKilometers" << YAML::Value << component.UnitsToKilometers;
			out << YAML::Key << "SeaLevelHeight" << YAML::Value << component.SeaLevelHeight;

			out << YAML::Key << "RayleighScattering" << YAML::Value << component.RayleighScattering;
			out << YAML::Key << "RayleighScaleHeight" << YAML::Value << component.RayleighScaleHeight;

			out << YAML::Key << "MieScattering" << YAML::Value << component.MieScattering;
			out << YAML::Key << "MieAbsorption" << YAML::Value << component.MieAbsorption;
			out << YAML::Key << "MieScaleHeight" << YAML::Value << component.MieScaleHeight;
			out << YAML::Key << "MieAnisotropy" << YAML::Value << component.MieAnisotropy;

			out << YAML::Key << "AbsorptionCoefficients" << YAML::Value << component.AbsorptionCoefficients;
			out << YAML::Key << "AbsorptionLayerCenter" << YAML::Value << component.AbsorptionLayerCenter;
			out << YAML::Key << "AbsorptionLayerWidth" << YAML::Value << component.AbsorptionLayerWidth;

			out << YAML::Key << "GroundColor" << YAML::Value << component.GroundColor;
			out << YAML::Key << "SkyIntensity" << YAML::Value << component.SkyIntensity;
			out << YAML::Key << "RenderSunDisks" << YAML::Value << component.RenderSunDisks;

			out << YAML::Key << "RenderStars" << YAML::Value << component.RenderStars;
			out << YAML::Key << "StarBrightness" << YAML::Value << component.StarBrightness;
			out << YAML::Key << "StarDensity" << YAML::Value << component.StarDensity;
			out << YAML::Key << "StarTwinkle" << YAML::Value << component.StarTwinkle;
			out << YAML::Key << "StarYearLength" << YAML::Value << component.StarYearLength;
			out << YAML::Key << "StarRotationSpeed" << YAML::Value << component.StarRotationSpeed;
			out << YAML::Key << "MilkyWayBrightness" << YAML::Value << component.MilkyWayBrightness;

			out << YAML::Key << "SkyLightUpdateInterval" << YAML::Value << component.SkyLightUpdateInterval;
			out << YAML::Key << "SkyLightUpdateAngle" << YAML::Value << component.SkyLightUpdateAngle;
		}

		void DeserializeData(const YAML::Node& node, AtmosphereComponent& component) override
		{
			int preset = static_cast<int>(AtmospherePreset::Earth);
			TryParse(node["Preset"], preset);
			component.Preset = static_cast<AtmospherePreset>(preset);

			TryParse(node["PlanetRadius"], component.PlanetRadius);
			TryParse(node["AtmosphereHeight"], component.AtmosphereHeight);
			TryParse(node["UnitsToKilometers"], component.UnitsToKilometers);
			TryParse(node["SeaLevelHeight"], component.SeaLevelHeight);

			TryParse(node["RayleighScattering"], component.RayleighScattering);
			TryParse(node["RayleighScaleHeight"], component.RayleighScaleHeight);

			TryParse(node["MieScattering"], component.MieScattering);
			TryParse(node["MieAbsorption"], component.MieAbsorption);
			TryParse(node["MieScaleHeight"], component.MieScaleHeight);
			TryParse(node["MieAnisotropy"], component.MieAnisotropy);

			TryParse(node["AbsorptionCoefficients"], component.AbsorptionCoefficients);
			TryParse(node["AbsorptionLayerCenter"], component.AbsorptionLayerCenter);
			TryParse(node["AbsorptionLayerWidth"], component.AbsorptionLayerWidth);

			TryParse(node["GroundColor"], component.GroundColor);
			TryParse(node["SkyIntensity"], component.SkyIntensity);
			TryParse(node["RenderSunDisks"], component.RenderSunDisks);

			TryParse(node["RenderStars"], component.RenderStars);
			TryParse(node["StarBrightness"], component.StarBrightness);
			TryParse(node["StarDensity"], component.StarDensity);
			TryParse(node["StarTwinkle"], component.StarTwinkle);
			TryParse(node["StarYearLength"], component.StarYearLength);
			TryParse(node["StarRotationSpeed"], component.StarRotationSpeed);
			TryParse(node["MilkyWayBrightness"], component.MilkyWayBrightness);

			TryParse(node["SkyLightUpdateInterval"], component.SkyLightUpdateInterval);
			TryParse(node["SkyLightUpdateAngle"], component.SkyLightUpdateAngle);
		}
	};

	class SunComponentSerializer final : public ComponentSerializer<SunComponent, SunComponentSerializer>
	{
	protected:
		const char* GetKey() override { return "Sun"; }

		void SerializeData(YAML::Emitter& out, const SunComponent& component) override
		{
			out << YAML::Key << "Color" << YAML::Value << component.Color;
			out << YAML::Key << "Intensity" << YAML::Value << component.Intensity;
			out << YAML::Key << "AngularDiameter" << YAML::Value << component.AngularDiameter;
			out << YAML::Key << "DrivesDirectionalLight" << YAML::Value << component.DrivesDirectionalLight;
			out << YAML::Key << "LightIntensity" << YAML::Value << component.LightIntensity;
		}

		void DeserializeData(const YAML::Node& node, SunComponent& component) override
		{
			TryParse(node["Color"], component.Color);
			TryParse(node["Intensity"], component.Intensity);
			TryParse(node["AngularDiameter"], component.AngularDiameter);
			TryParse(node["DrivesDirectionalLight"], component.DrivesDirectionalLight);
			TryParse(node["LightIntensity"], component.LightIntensity);
		}
	};

	class MoonComponentSerializer final : public ComponentSerializer<MoonComponent, MoonComponentSerializer>
	{
	protected:
		const char* GetKey() override { return "Moon"; }

		void SerializeData(YAML::Emitter& out, const MoonComponent& component) override
		{
			out << YAML::Key << "Color" << YAML::Value << component.Color;
			out << YAML::Key << "Reflectance" << YAML::Value << component.Reflectance;
			out << YAML::Key << "AngularDiameter" << YAML::Value << component.AngularDiameter;
			out << YAML::Key << "DiskBrightness" << YAML::Value << component.DiskBrightness;
			out << YAML::Key << "PhaseInfluence" << YAML::Value << component.PhaseInfluence;
			out << YAML::Key << "DrivesDirectionalLight" << YAML::Value << component.DrivesDirectionalLight;
			out << YAML::Key << "LightIntensity" << YAML::Value << component.LightIntensity;
		}

		void DeserializeData(const YAML::Node& node, MoonComponent& component) override
		{
			TryParse(node["Color"], component.Color);
			TryParse(node["Reflectance"], component.Reflectance);
			TryParse(node["AngularDiameter"], component.AngularDiameter);
			TryParse(node["DiskBrightness"], component.DiskBrightness);
			TryParse(node["PhaseInfluence"], component.PhaseInfluence);
			TryParse(node["DrivesDirectionalLight"], component.DrivesDirectionalLight);
			TryParse(node["LightIntensity"], component.LightIntensity);
		}
	};

	class CelestialOrbitComponentSerializer final : public ComponentSerializer<
			CelestialOrbitComponent, CelestialOrbitComponentSerializer>
	{
	protected:
		const char* GetKey() override { return "Celestial Orbit"; }

		void SerializeData(YAML::Emitter& out, const CelestialOrbitComponent& component) override
		{
			out << YAML::Key << "DayLength" << YAML::Value << component.DayLength;
			out << YAML::Key << "TimeOfDay" << YAML::Value << component.TimeOfDay;
			out << YAML::Key << "DaysElapsed" << YAML::Value << component.DaysElapsed;
			out << YAML::Key << "AutoAdvance" << YAML::Value << component.AutoAdvance;
			out << YAML::Key << "Inclination" << YAML::Value << component.Inclination;
			out << YAML::Key << "NorthOffset" << YAML::Value << component.NorthOffset;
		}

		void DeserializeData(const YAML::Node& node, CelestialOrbitComponent& component) override
		{
			TryParse(node["DayLength"], component.DayLength);
			TryParse(node["TimeOfDay"], component.TimeOfDay);
			TryParse(node["DaysElapsed"], component.DaysElapsed);
			TryParse(node["AutoAdvance"], component.AutoAdvance);
			TryParse(node["Inclination"], component.Inclination);
			TryParse(node["NorthOffset"], component.NorthOffset);
		}
	};
}
