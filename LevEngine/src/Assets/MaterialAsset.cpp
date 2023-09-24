#include "levpch.h"
#include "MaterialAsset.h"

#include "Renderer/Material.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	void MaterialAsset::SerializeData(YAML::Emitter& out)
	{
		out << YAML::Key << "AmbientColor" << YAML::Value << material.GetAmbientColor();
		out << YAML::Key << "DiffuseColor" << YAML::Value << material.GetDiffuseColor();
		out << YAML::Key << "SpecularColor" << YAML::Value << material.GetSpecularColor();
		out << YAML::Key << "EmissiveColor" << YAML::Value << material.GetEmissiveColor();

		out << YAML::Key << "Shininess" << YAML::Value << material.GetShininess();

		SerializeAsset(out, "Diffuse", diffuse);
		SerializeAsset(out, "Emissive", emissive);
		SerializeAsset(out, "Specular", specular);
		SerializeAsset(out, "Normal", normal);

		out << YAML::Key << "Tiling" << YAML::Value << material.GetTextureTiling();
		out << YAML::Key << "Offset" << YAML::Value << material.GetTextureOffset();
	}
	void MaterialAsset::DeserializeData(YAML::Node& node)
	{
		material = {};

		material.SetAmbientColor(node["AmbientColor"].as<Color>());
		material.SetDiffuseColor(node["DiffuseColor"].as<Color>());
		material.SetSpecularColor(node["SpecularColor"].as<Color>());
		material.SetEmissiveColor(node["EmissiveColor"].as<Color>());
		material.SetShininess(node["Shininess"].as<float>());

		diffuse = DeserializeAsset<TextureAsset>(node["Diffuse"]);
		emissive = DeserializeAsset<TextureAsset>(node["Emissive"]);
		specular = DeserializeAsset<TextureAsset>(node["Specular"]);
		normal = DeserializeAsset<TextureAsset>(node["Normal"]);

		if (diffuse)
			material.SetTexture(Material::TextureType::Diffuse, diffuse->GetTexture());

		if (emissive)
			material.SetTexture(Material::TextureType::Emissive, emissive->GetTexture());

		if (specular)
			material.SetTexture(Material::TextureType::Specular, specular->GetTexture());

		if (normal)
			material.SetTexture(Material::TextureType::Normal, normal->GetTexture());

		material.SetTextureTiling(node["Tiling"].as<Vector2>());
		material.SetTextureOffset(node["Offset"].as<Vector2>());
	}
}
