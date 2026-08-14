#include "levpch.h"
#include "PlanetBiomeSetAsset.h"

#include "EngineAssets.h"
#include "TextureAsset.h"
#include "Renderer/Pipeline/Texture.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	const Ref<Texture>& PlanetBiomeSetAsset::GetAlbedoArray()
	{
		BuildArrays();
		return m_AlbedoArray;
	}

	const Ref<Texture>& PlanetBiomeSetAsset::GetNormalArray()
	{
		BuildArrays();
		return m_NormalArray;
	}

	const Ref<Texture>& PlanetBiomeSetAsset::GetRoughnessArray()
	{
		BuildArrays();
		return m_RoughnessArray;
	}

	void PlanetBiomeSetAsset::InvalidateTextures()
	{
		m_AlbedoArray.reset();
		m_NormalArray.reset();
		m_RoughnessArray.reset();
		m_ArraysBuilt = false;
	}

	void PlanetBiomeSetAsset::BuildArrays()
	{
		if (m_ArraysBuilt) return;

		m_ArraysBuilt = true;

		if (m_Textures.empty()) return;

		m_AlbedoArray = BuildArray(Channel::Albedo);
		m_NormalArray = BuildArray(Channel::Normal);
		m_RoughnessArray = BuildArray(Channel::Roughness);
	}

	Ref<Texture> PlanetBiomeSetAsset::BuildArray(const Channel channel) const
	{
		Vector<String> paths;
		paths.reserve(m_Textures.size());

		bool anyAssigned = false;

		for (const GroundTextures& textures : m_Textures)
		{
			Ref<TextureAsset> asset;

			switch (channel)
			{
			case Channel::Albedo: asset = textures.Albedo; break;
			case Channel::Normal: asset = textures.Normal; break;
			case Channel::Roughness: asset = textures.Roughness; break;
			}

			// An unassigned slot still takes a slice. Skipping it would shift every slice after it and
			// silently repaint the biomes that index them.
			if (asset)
			{
				paths.emplace_back(asset->GetPath().string().c_str());
				anyAssigned = true;
			}
			else
			{
				paths.emplace_back(String{});
			}
		}

		if (!anyAssigned) return nullptr;

		// Albedo is colour and wants the sRGB decode. Normal and roughness are measurements -- a
		// normal put through a gamma curve points somewhere else, and roughness put through one is
		// the wrong roughness.
		const bool isLinear = channel != Channel::Albedo;

		return Texture::CreateTexture2DArray(paths, isLinear, true);
	}

	void PlanetBiomeSetAsset::ResetToEarthlike()
	{
		m_Biomes = PlanetBiomeTable::CreateEarthlikeSet();

		// One texture slot per biome, named after it and empty. The set is usable immediately from the
		// biome tints, and the slots are there to be filled in rather than to be created one by one.
		m_Textures.clear();
		m_Textures.reserve(m_Biomes.size());

		for (size_t biome = 0; biome < m_Biomes.size(); ++biome)
		{
			GroundTextures textures;
			textures.Name = m_Biomes[biome].Name;

			m_Textures.emplace_back(textures);

			m_Biomes[biome].TextureIndex = static_cast<int32_t>(biome);
		}

		InvalidateTextures();
	}

	Ref<Texture> PlanetBiomeSetAsset::GetIcon() const { return Icons::Material(); }

	void PlanetBiomeSetAsset::SerializeData(YAML::Emitter& out)
	{
		out << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;

		for (const GroundTextures& textures : m_Textures)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << textures.Name;
			SerializeAsset(out, "Albedo", textures.Albedo);
			SerializeAsset(out, "Normal", textures.Normal);
			SerializeAsset(out, "Roughness", textures.Roughness);
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		out << YAML::Key << "Biomes" << YAML::Value << YAML::BeginSeq;

		for (const PlanetBiome& biome : m_Biomes)
		{
			out << YAML::BeginMap;

			out << YAML::Key << "Name" << YAML::Value << biome.Name;

			out << YAML::Key << "MinTemperature" << YAML::Value << biome.MinTemperature;
			out << YAML::Key << "MaxTemperature" << YAML::Value << biome.MaxTemperature;
			out << YAML::Key << "MinHumidity" << YAML::Value << biome.MinHumidity;
			out << YAML::Key << "MaxHumidity" << YAML::Value << biome.MaxHumidity;
			out << YAML::Key << "MinHeight" << YAML::Value << biome.MinHeight;
			out << YAML::Key << "MaxHeight" << YAML::Value << biome.MaxHeight;
			out << YAML::Key << "MinSlope" << YAML::Value << biome.MinSlope;
			out << YAML::Key << "MaxSlope" << YAML::Value << biome.MaxSlope;

			out << YAML::Key << "TemperatureBlend" << YAML::Value << biome.TemperatureBlend;
			out << YAML::Key << "HumidityBlend" << YAML::Value << biome.HumidityBlend;
			out << YAML::Key << "HeightBlend" << YAML::Value << biome.HeightBlend;
			out << YAML::Key << "SlopeBlend" << YAML::Value << biome.SlopeBlend;

			out << YAML::Key << "Tint" << YAML::Value << biome.Tint;
			out << YAML::Key << "Roughness" << YAML::Value << biome.Roughness;
			out << YAML::Key << "Metallic" << YAML::Value << biome.Metallic;
			out << YAML::Key << "TextureScale" << YAML::Value << biome.TextureScale;
			out << YAML::Key << "TextureIndex" << YAML::Value << biome.TextureIndex;
			out << YAML::Key << "NormalStrength" << YAML::Value << biome.NormalStrength;
			out << YAML::Key << "Priority" << YAML::Value << biome.Priority;

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
	}

	void PlanetBiomeSetAsset::DeserializeData(const YAML::Node& node)
	{
		m_Textures.clear();
		m_Biomes.clear();

		if (const auto& textureNodes = node["Textures"])
		{
			for (const auto& textureNode : textureNodes)
			{
				GroundTextures textures;

				TryParse(textureNode["Name"], textures.Name);
				textures.Albedo = DeserializeAsset<TextureAsset>(textureNode["Albedo"]);
				textures.Normal = DeserializeAsset<TextureAsset>(textureNode["Normal"]);
				textures.Roughness = DeserializeAsset<TextureAsset>(textureNode["Roughness"]);

				m_Textures.emplace_back(textures);
			}
		}

		if (const auto& biomeNodes = node["Biomes"])
		{
			for (const auto& biomeNode : biomeNodes)
			{
				PlanetBiome biome;

				TryParse(biomeNode["Name"], biome.Name);

				TryParse(biomeNode["MinTemperature"], biome.MinTemperature);
				TryParse(biomeNode["MaxTemperature"], biome.MaxTemperature);
				TryParse(biomeNode["MinHumidity"], biome.MinHumidity);
				TryParse(biomeNode["MaxHumidity"], biome.MaxHumidity);
				TryParse(biomeNode["MinHeight"], biome.MinHeight);
				TryParse(biomeNode["MaxHeight"], biome.MaxHeight);
				TryParse(biomeNode["MinSlope"], biome.MinSlope);
				TryParse(biomeNode["MaxSlope"], biome.MaxSlope);

				TryParse(biomeNode["TemperatureBlend"], biome.TemperatureBlend);
				TryParse(biomeNode["HumidityBlend"], biome.HumidityBlend);
				TryParse(biomeNode["HeightBlend"], biome.HeightBlend);
				TryParse(biomeNode["SlopeBlend"], biome.SlopeBlend);

				TryParse(biomeNode["Tint"], biome.Tint);
				TryParse(biomeNode["Roughness"], biome.Roughness);
				TryParse(biomeNode["Metallic"], biome.Metallic);
				TryParse(biomeNode["TextureScale"], biome.TextureScale);
				TryParse(biomeNode["TextureIndex"], biome.TextureIndex);
				TryParse(biomeNode["NormalStrength"], biome.NormalStrength);
				TryParse(biomeNode["Priority"], biome.Priority);

				m_Biomes.emplace_back(biome);
			}
		}

		// An empty file is a new asset rather than a broken one -- CreateNewAsset writes the file
		// before anything has been put in it -- so it gets the default set instead of no ground.
		if (m_Biomes.empty())
			ResetToEarthlike();

		InvalidateTextures();
	}
}
