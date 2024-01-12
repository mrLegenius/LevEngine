#include "levpch.h"
#include "MaterialSimpleAsset.h"
#include "TextureAsset.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	void MaterialSimpleAsset::SerializeData(YAML::Emitter& out)
	{
		out << YAML::Key << "AmbientColor" << YAML::Value << m_Material.GetAmbientColor();
		out << YAML::Key << "DiffuseColor" << YAML::Value << m_Material.GetDiffuseColor();
		out << YAML::Key << "SpecularColor" << YAML::Value << m_Material.GetSpecularColor();
		out << YAML::Key << "EmissiveColor" << YAML::Value << m_Material.GetEmissiveColor();

		out << YAML::Key << "Shininess" << YAML::Value << m_Material.GetShininess();

		SerializeAsset(out, "Diffuse", m_Diffuse);
		SerializeAsset(out, "Emissive", m_Emissive);
		SerializeAsset(out, "Specular", m_Specular);
		SerializeAsset(out, "Normal", m_Normal);

		out << YAML::Key << "Tiling" << YAML::Value << m_Material.GetTextureTiling();
		out << YAML::Key << "Offset" << YAML::Value << m_Material.GetTextureOffset();
	}
	void MaterialSimpleAsset::DeserializeData(const YAML::Node& node)
	{
		m_Material = {};

		m_Material.SetAmbientColor(node["AmbientColor"].as<Color>());
		m_Material.SetDiffuseColor(node["DiffuseColor"].as<Color>());
		m_Material.SetSpecularColor(node["SpecularColor"].as<Color>());
		m_Material.SetEmissiveColor(node["EmissiveColor"].as<Color>());
		m_Material.SetShininess(node["Shininess"].as<float>());

		m_Diffuse = DeserializeAsset<TextureAsset>(node["Diffuse"]);
		m_Emissive = DeserializeAsset<TextureAsset>(node["Emissive"]);
		m_Specular = DeserializeAsset<TextureAsset>(node["Specular"]);
		m_Normal = DeserializeAsset<TextureAsset>(node["Normal"]);

		if (m_Diffuse)
			m_Material.SetTexture(MaterialSimple::TextureType::Diffuse, m_Diffuse->GetTexture());

		if (m_Emissive)
			m_Material.SetTexture(MaterialSimple::TextureType::Emissive, m_Emissive->GetTexture());

		if (m_Specular)
			m_Material.SetTexture(MaterialSimple::TextureType::Specular, m_Specular->GetTexture());

		if (m_Normal)
			m_Material.SetTexture(MaterialSimple::TextureType::Normal, m_Normal->GetTexture());

		m_Material.SetTextureTiling(node["Tiling"].as<Vector2>());
		m_Material.SetTextureOffset(node["Offset"].as<Vector2>());
	}
}
