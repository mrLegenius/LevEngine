#include "levpch.h"
#include "MaterialSimpleAsset.h"

#include <imgui.h>

#include "TextureLibrary.h"
#include "GUI/GUIUtils.h"
#include "Renderer/MaterialSimple.h"
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
	void MaterialSimpleAsset::DeserializeData(YAML::Node& node)
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

	void MaterialSimpleAsset::DrawProperties()
	{
		GUIUtils::DrawColor3Control("Ambient", BindGetter(&MaterialSimple::GetAmbientColor, &m_Material), BindSetter(&MaterialSimple::SetAmbientColor, &m_Material));
		GUIUtils::DrawColor3Control("Diffuse", BindGetter(&MaterialSimple::GetDiffuseColor, &m_Material), BindSetter(&MaterialSimple::SetDiffuseColor, &m_Material));
		GUIUtils::DrawColor3Control("Specular", BindGetter(&MaterialSimple::GetSpecularColor, &m_Material), BindSetter(&MaterialSimple::SetSpecularColor, &m_Material));
		GUIUtils::DrawColor3Control("Emissive", BindGetter(&MaterialSimple::GetEmissiveColor, &m_Material), BindSetter(&MaterialSimple::SetEmissiveColor, &m_Material));

		GUIUtils::DrawFloatControl("Shininess", BindGetter(&MaterialSimple::GetShininess, &m_Material), BindSetter(&MaterialSimple::SetShininess, &m_Material), 1.0f, 2.0f, 128.0f);

		DrawMaterialTexture("Diffuse", m_Material, MaterialSimple::TextureType::Diffuse, m_Diffuse);
		DrawMaterialTexture("Specular", m_Material, MaterialSimple::TextureType::Specular, m_Specular);
		DrawMaterialTexture("Normal", m_Material, MaterialSimple::TextureType::Normal, m_Normal);
		DrawMaterialTexture("Emissive", m_Material, MaterialSimple::TextureType::Emissive, m_Emissive);

		auto tiling = m_Material.GetTextureTiling();
		if (GUIUtils::DrawVector2Control("Tiling", tiling, 1, 100))
			m_Material.SetTextureTiling(tiling);

		auto offset = m_Material.GetTextureOffset();
		if (GUIUtils::DrawVector2Control("Offset", offset, 0, 100))
			m_Material.SetTextureOffset(offset);
	}
	void MaterialSimpleAsset::DrawMaterialTexture(const String& label, MaterialSimple& material,
	                                        MaterialSimple::TextureType textureType, Ref<TextureAsset>& textureAsset)
	{
		ImGui::PushID(static_cast<int>(textureType));

		if (GUIUtils::DrawTextureAsset(label, &textureAsset))
			material.SetTexture(textureType, textureAsset ? textureAsset->GetTexture() : nullptr);
		ImGui::PopID();
	}
}
