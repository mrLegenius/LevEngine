#include "levpch.h"
#include "Planet.h"

#include "../ComponentSerializer.h"
#include "Assets/PlanetBiomeSetAsset.h"

namespace LevEngine
{
	namespace
	{
		//<--- What a planet with no biome set is drawn with. See PlanetComponent::GetBiomes ---<<
		const Vector<PlanetBiome>& GetDefaultBiomes()
		{
			static const Vector<PlanetBiome> biomes = PlanetBiomeTable::CreateEarthlikeSet();
			return biomes;
		}
	}

	PlanetComponent::PlanetComponent()
		: Surface(CreateRef<PlanetSurface>())
	{
		ApplySettings();
	}

	void PlanetComponent::ApplySettings() const
	{
		if (!Surface) return;

		PlanetLodSettings lod = Lod;

		// Collision reads the triangles of a chunk back after it is built, so the generator has to be
		// told to keep them. Deriving it here rather than asking the author to keep two switches in
		// step: a planet with collision off has no reason to hold on to any of it.
		lod.KeepGeometryForCollision = GenerateCollision;

		Surface->Configure(Shape, Climate, lod);
	}

	const Vector<PlanetBiome>& PlanetComponent::GetBiomes() const
	{
		if (BiomeSet && !BiomeSet->GetBiomes().empty())
			return BiomeSet->GetBiomes();

		return GetDefaultBiomes();
	}

	void PlanetPresets::Apply(PlanetComponent& component, const PlanetPreset preset)
	{
		//<--- Every preset starts from the defaults, so each only states what it changes ---<<
		const float radius = component.Shape.Radius;
		const uint32_t seed = component.Shape.Seed;

		component.Shape = PlanetShapeSettings{};
		component.Climate = PlanetClimateSettings{};
		component.Ocean = PlanetOceanSettings{};

		component.Shape.Radius = radius;
		component.Shape.Seed = seed;

		switch (preset)
		{
		case PlanetPreset::Earthlike:
			break;

		case PlanetPreset::Desert:
			// Dry from both ends: hardly any sea to evaporate off, and a circulation too weak to carry
			// what little there is inland. The few wet places left are the poles.
			component.Shape.SeaLevelThreshold = 0.6f;
			component.Shape.OceanDepth = 110.0f;
			component.Shape.MountainHeight = 320.0f;
			component.Shape.DetailHeight = 20.0f;

			component.Climate.EquatorTemperature = 46.0f;
			component.Climate.PoleTemperature = -8.0f;
			component.Climate.HumidityAtCoast = 0.45f;
			component.Climate.HumidityInland = 0.02f;
			component.Climate.CirculationStrength = 0.35f;
			component.Climate.RainShadowStrength = 0.8f;

			component.Ocean.Render = true;
			break;

		case PlanetPreset::IceWorld:
			// Cold enough that the snow biome wins everywhere below the tropics. The sea is left in
			// because the interesting part of an ice world is the coast.
			component.Climate.EquatorTemperature = -4.0f;
			component.Climate.PoleTemperature = -58.0f;
			component.Climate.LapseRate = 5.0f;
			component.Climate.HumidityAtCoast = 0.7f;

			component.Ocean.ShallowColor = Color(0.4f, 0.62f, 0.68f, 1.0f);
			component.Ocean.DeepColor = Color(0.05f, 0.14f, 0.24f, 1.0f);
			component.Ocean.WaveStrength = 0.15f;
			break;

		case PlanetPreset::OceanWorld:
			// Mostly water, with what land there is left in island arcs -- which is what the belt
			// field does once the shoreline is below most of the continent field.
			component.Shape.SeaLevelThreshold = -0.1f;
			component.Shape.OceanDepth = 520.0f;
			component.Shape.MountainHeight = 220.0f;
			component.Shape.MountainInlandBias = 0.2f;
			component.Shape.ShoreWidth = 0.08f;

			component.Climate.HumidityInland = 0.55f;
			break;

		case PlanetPreset::Volcanic:
			// No water at all, so the climate has nothing to rain: everything above the sea floor is
			// rock, and the relief is where the interest is.
			component.Shape.SeaLevelThreshold = -0.6f;
			component.Shape.MountainHeight = 620.0f;
			component.Shape.MountainBeltWidth = 0.55f;
			component.Shape.DetailHeight = 32.0f;
			component.Shape.DetailReliefBias = 0.3f;

			component.Climate.EquatorTemperature = 78.0f;
			component.Climate.PoleTemperature = 26.0f;
			component.Climate.HumidityAtCoast = 0.08f;
			component.Climate.HumidityInland = 0.0f;
			component.Climate.CirculationStrength = 0.1f;

			component.Ocean.Render = false;
			break;

		case PlanetPreset::Custom:
			return;
		}

		component.Preset = preset;
		component.ApplySettings();
	}

	const char* PlanetPresets::GetName(const PlanetPreset preset)
	{
		switch (preset)
		{
		case PlanetPreset::Earthlike: return "Earthlike";
		case PlanetPreset::Desert: return "Desert";
		case PlanetPreset::IceWorld: return "Ice World";
		case PlanetPreset::OceanWorld: return "Ocean World";
		case PlanetPreset::Volcanic: return "Volcanic";
		case PlanetPreset::Custom: return "Custom";
		}

		return "Unknown";
	}

	namespace
	{
		void SerializeFractal(YAML::Emitter& out, const String& name, const Noise::FractalSettings& settings)
		{
			out << YAML::Key << name << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "Frequency" << YAML::Value << settings.Frequency;
			out << YAML::Key << "Octaves" << YAML::Value << settings.Octaves;
			out << YAML::Key << "Lacunarity" << YAML::Value << settings.Lacunarity;
			out << YAML::Key << "Gain" << YAML::Value << settings.Gain;
			out << YAML::Key << "Seed" << YAML::Value << settings.Seed;
			out << YAML::EndMap;
		}

		void DeserializeFractal(const YAML::Node& node, Noise::FractalSettings& settings)
		{
			if (!node) return;

			TryParse(node["Frequency"], settings.Frequency);
			TryParse(node["Octaves"], settings.Octaves);
			TryParse(node["Lacunarity"], settings.Lacunarity);
			TryParse(node["Gain"], settings.Gain);
			TryParse(node["Seed"], settings.Seed);
		}
	}

	class PlanetComponentSerializer final : public ComponentSerializer<PlanetComponent, PlanetComponentSerializer>
	{
	protected:
		const char* GetKey() override { return "Planet"; }

		void SerializeData(YAML::Emitter& out, const PlanetComponent& component) override
		{
			out << YAML::Key << "Preset" << YAML::Value << static_cast<int>(component.Preset);

			//<--- Shape ---<<
			out << YAML::Key << "Seed" << YAML::Value << component.Shape.Seed;
			out << YAML::Key << "Radius" << YAML::Value << component.Shape.Radius;
			out << YAML::Key << "SeaLevelThreshold" << YAML::Value << component.Shape.SeaLevelThreshold;
			out << YAML::Key << "ShoreWidth" << YAML::Value << component.Shape.ShoreWidth;
			out << YAML::Key << "OceanDepth" << YAML::Value << component.Shape.OceanDepth;
			out << YAML::Key << "LandHeight" << YAML::Value << component.Shape.LandHeight;
			out << YAML::Key << "ContinentWarp" << YAML::Value << component.Shape.ContinentWarp;
			out << YAML::Key << "ContinentWarpFrequency" << YAML::Value << component.Shape.ContinentWarpFrequency;
			out << YAML::Key << "MountainHeight" << YAML::Value << component.Shape.MountainHeight;
			out << YAML::Key << "MountainBeltWidth" << YAML::Value << component.Shape.MountainBeltWidth;
			out << YAML::Key << "MountainInlandBias" << YAML::Value << component.Shape.MountainInlandBias;
			out << YAML::Key << "DetailHeight" << YAML::Value << component.Shape.DetailHeight;
			out << YAML::Key << "DetailReliefBias" << YAML::Value << component.Shape.DetailReliefBias;

			SerializeFractal(out, "Continents", component.Shape.Continents);
			SerializeFractal(out, "Mountains", component.Shape.Mountains);
			SerializeFractal(out, "MountainBelts", component.Shape.MountainBelts);
			SerializeFractal(out, "ShapeDetail", component.Shape.Detail);

			//<--- Climate ---<<
			out << YAML::Key << "ClimateSeed" << YAML::Value << component.Climate.Seed;
			out << YAML::Key << "EquatorTemperature" << YAML::Value << component.Climate.EquatorTemperature;
			out << YAML::Key << "PoleTemperature" << YAML::Value << component.Climate.PoleTemperature;
			out << YAML::Key << "LatitudeFalloff" << YAML::Value << component.Climate.LatitudeFalloff;
			out << YAML::Key << "LapseRate" << YAML::Value << component.Climate.LapseRate;
			out << YAML::Key << "KilometersPerUnit" << YAML::Value << component.Climate.KilometersPerUnit;
			out << YAML::Key << "TemperatureVariationRange" << YAML::Value << component.Climate.TemperatureVariationRange;
			out << YAML::Key << "HumidityAtCoast" << YAML::Value << component.Climate.HumidityAtCoast;
			out << YAML::Key << "HumidityInland" << YAML::Value << component.Climate.HumidityInland;
			out << YAML::Key << "CirculationStrength" << YAML::Value << component.Climate.CirculationStrength;
			out << YAML::Key << "HumidityVariationRange" << YAML::Value << component.Climate.HumidityVariationRange;
			out << YAML::Key << "RainShadowStrength" << YAML::Value << component.Climate.RainShadowStrength;
			out << YAML::Key << "RainShadowArc" << YAML::Value << component.Climate.RainShadowArc;
			out << YAML::Key << "RainShadowRelief" << YAML::Value << component.Climate.RainShadowRelief;
			out << YAML::Key << "AltitudeDrying" << YAML::Value << component.Climate.AltitudeDrying;

			SerializeFractal(out, "TemperatureVariation", component.Climate.TemperatureVariation);
			SerializeFractal(out, "HumidityVariation", component.Climate.HumidityVariation);

			//<--- Level of detail ---<<
			out << YAML::Key << "ChunkResolution" << YAML::Value << component.Lod.ChunkResolution;
			out << YAML::Key << "MaxDepth" << YAML::Value << component.Lod.MaxDepth;
			out << YAML::Key << "LodBias" << YAML::Value << component.Lod.LodBias;
			out << YAML::Key << "SkirtDepthScale" << YAML::Value << component.Lod.SkirtDepthScale;
			out << YAML::Key << "MaxConcurrentBuilds" << YAML::Value << component.Lod.MaxConcurrentBuilds;

			//<--- Ocean ---<<
			out << YAML::Key << "RenderOcean" << YAML::Value << component.Ocean.Render;
			out << YAML::Key << "OceanShallowColor" << YAML::Value << component.Ocean.ShallowColor;
			out << YAML::Key << "OceanDeepColor" << YAML::Value << component.Ocean.DeepColor;
			out << YAML::Key << "OceanDepthFalloff" << YAML::Value << component.Ocean.DepthFalloff;
			out << YAML::Key << "OceanRoughness" << YAML::Value << component.Ocean.Roughness;
			out << YAML::Key << "OceanOpacity" << YAML::Value << component.Ocean.Opacity;
			out << YAML::Key << "OceanFresnelStrength" << YAML::Value << component.Ocean.FresnelStrength;
			out << YAML::Key << "WaveStrength" << YAML::Value << component.Ocean.WaveStrength;
			out << YAML::Key << "WaveScale" << YAML::Value << component.Ocean.WaveScale;
			out << YAML::Key << "WaveSpeed" << YAML::Value << component.Ocean.WaveSpeed;

			//<--- Shader side detail ---<<
			out << YAML::Key << "DetailStrength" << YAML::Value << component.Detail.Strength;
			out << YAML::Key << "DetailFrequency" << YAML::Value << component.Detail.Frequency;
			out << YAML::Key << "TriplanarSharpness" << YAML::Value << component.Detail.TriplanarSharpness;
			out << YAML::Key << "DetailFadeStart" << YAML::Value << component.Detail.FadeStart;
			out << YAML::Key << "DetailFadeEnd" << YAML::Value << component.Detail.FadeEnd;

			out << YAML::Key << "GenerateCollision" << YAML::Value << component.GenerateCollision;
			out << YAML::Key << "CollisionRadius" << YAML::Value << component.CollisionRadius;

			SerializeAsset(out, "BiomeSet", component.BiomeSet);
		}

		void DeserializeData(const YAML::Node& node, PlanetComponent& component) override
		{
			int preset = static_cast<int>(PlanetPreset::Earthlike);
			TryParse(node["Preset"], preset);
			component.Preset = static_cast<PlanetPreset>(preset);

			TryParse(node["Seed"], component.Shape.Seed);
			TryParse(node["Radius"], component.Shape.Radius);
			TryParse(node["SeaLevelThreshold"], component.Shape.SeaLevelThreshold);
			TryParse(node["ShoreWidth"], component.Shape.ShoreWidth);
			TryParse(node["OceanDepth"], component.Shape.OceanDepth);
			TryParse(node["LandHeight"], component.Shape.LandHeight);
			TryParse(node["ContinentWarp"], component.Shape.ContinentWarp);
			TryParse(node["ContinentWarpFrequency"], component.Shape.ContinentWarpFrequency);
			TryParse(node["MountainHeight"], component.Shape.MountainHeight);
			TryParse(node["MountainBeltWidth"], component.Shape.MountainBeltWidth);
			TryParse(node["MountainInlandBias"], component.Shape.MountainInlandBias);
			TryParse(node["DetailHeight"], component.Shape.DetailHeight);
			TryParse(node["DetailReliefBias"], component.Shape.DetailReliefBias);

			DeserializeFractal(node["Continents"], component.Shape.Continents);
			DeserializeFractal(node["Mountains"], component.Shape.Mountains);
			DeserializeFractal(node["MountainBelts"], component.Shape.MountainBelts);
			DeserializeFractal(node["ShapeDetail"], component.Shape.Detail);

			TryParse(node["ClimateSeed"], component.Climate.Seed);
			TryParse(node["EquatorTemperature"], component.Climate.EquatorTemperature);
			TryParse(node["PoleTemperature"], component.Climate.PoleTemperature);
			TryParse(node["LatitudeFalloff"], component.Climate.LatitudeFalloff);
			TryParse(node["LapseRate"], component.Climate.LapseRate);
			TryParse(node["KilometersPerUnit"], component.Climate.KilometersPerUnit);
			TryParse(node["TemperatureVariationRange"], component.Climate.TemperatureVariationRange);
			TryParse(node["HumidityAtCoast"], component.Climate.HumidityAtCoast);
			TryParse(node["HumidityInland"], component.Climate.HumidityInland);
			TryParse(node["CirculationStrength"], component.Climate.CirculationStrength);
			TryParse(node["HumidityVariationRange"], component.Climate.HumidityVariationRange);
			TryParse(node["RainShadowStrength"], component.Climate.RainShadowStrength);
			TryParse(node["RainShadowArc"], component.Climate.RainShadowArc);
			TryParse(node["RainShadowRelief"], component.Climate.RainShadowRelief);
			TryParse(node["AltitudeDrying"], component.Climate.AltitudeDrying);

			DeserializeFractal(node["TemperatureVariation"], component.Climate.TemperatureVariation);
			DeserializeFractal(node["HumidityVariation"], component.Climate.HumidityVariation);

			TryParse(node["ChunkResolution"], component.Lod.ChunkResolution);
			TryParse(node["MaxDepth"], component.Lod.MaxDepth);
			TryParse(node["LodBias"], component.Lod.LodBias);
			TryParse(node["SkirtDepthScale"], component.Lod.SkirtDepthScale);
			TryParse(node["MaxConcurrentBuilds"], component.Lod.MaxConcurrentBuilds);

			TryParse(node["RenderOcean"], component.Ocean.Render);
			TryParse(node["OceanShallowColor"], component.Ocean.ShallowColor);
			TryParse(node["OceanDeepColor"], component.Ocean.DeepColor);
			TryParse(node["OceanDepthFalloff"], component.Ocean.DepthFalloff);
			TryParse(node["OceanRoughness"], component.Ocean.Roughness);
			TryParse(node["OceanOpacity"], component.Ocean.Opacity);
			TryParse(node["OceanFresnelStrength"], component.Ocean.FresnelStrength);
			TryParse(node["WaveStrength"], component.Ocean.WaveStrength);
			TryParse(node["WaveScale"], component.Ocean.WaveScale);
			TryParse(node["WaveSpeed"], component.Ocean.WaveSpeed);

			TryParse(node["DetailStrength"], component.Detail.Strength);
			TryParse(node["DetailFrequency"], component.Detail.Frequency);
			TryParse(node["TriplanarSharpness"], component.Detail.TriplanarSharpness);
			TryParse(node["DetailFadeStart"], component.Detail.FadeStart);
			TryParse(node["DetailFadeEnd"], component.Detail.FadeEnd);

			TryParse(node["GenerateCollision"], component.GenerateCollision);
			TryParse(node["CollisionRadius"], component.CollisionRadius);

			component.BiomeSet = DeserializeAsset<PlanetBiomeSetAsset>(node["BiomeSet"]);

			//<--- The tree is built from the settings, so it can only be set up once they are all in ---<<
			component.ApplySettings();
		}
	};
}
