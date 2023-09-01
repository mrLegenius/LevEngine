#include "levpch.h"
#include "MaterialAsset.h"

#include <imgui.h>

#include "TextureLibrary.h"
#include "GUI/GUIUtils.h"
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

		SerializeAsset(out, "Diffuse", m_Diffuse);
		SerializeAsset(out, "Emissive", m_Emissive);
		SerializeAsset(out, "Specular", m_Specular);
		SerializeAsset(out, "Normal", m_Normal);

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

		m_Diffuse = DeserializeAsset<TextureAsset>(node["Diffuse"]);
		m_Emissive = DeserializeAsset<TextureAsset>(node["Emissive"]);
		m_Specular = DeserializeAsset<TextureAsset>(node["Specular"]);
		m_Normal = DeserializeAsset<TextureAsset>(node["Normal"]);

		if (m_Diffuse)
			material.SetTexture(Material::TextureType::Diffuse, m_Diffuse->GetTexture());

		if (m_Emissive)
			material.SetTexture(Material::TextureType::Emissive, m_Emissive->GetTexture());

		if (m_Specular)
			material.SetTexture(Material::TextureType::Specular, m_Specular->GetTexture());

		if (m_Normal)
			material.SetTexture(Material::TextureType::Normal, m_Normal->GetTexture());

		material.SetTextureTiling(node["Tiling"].as<Vector2>());
		material.SetTextureOffset(node["Offset"].as<Vector2>());
	}

	void MaterialAsset::DrawProperties()
	{
		GUIUtils::DrawColor3Control("Ambient", BindGetter(&Material::GetAmbientColor, &material), BindSetter(&Material::SetAmbientColor, &material));
		GUIUtils::DrawColor3Control("Diffuse", BindGetter(&Material::GetDiffuseColor, &material), BindSetter(&Material::SetDiffuseColor, &material));
		GUIUtils::DrawColor3Control("Specular", BindGetter(&Material::GetSpecularColor, &material), BindSetter(&Material::SetSpecularColor, &material));
		GUIUtils::DrawColor3Control("Emissive", BindGetter(&Material::GetEmissiveColor, &material), BindSetter(&Material::SetEmissiveColor, &material));

		GUIUtils::DrawFloatControl("Shininess", BindGetter(&Material::GetShininess, &material), BindSetter(&Material::SetShininess, &material), 1.0f, 2.0f, 128.0f);

		DrawMaterialTexture("Diffuse", material, Material::TextureType::Diffuse, m_Diffuse);
		DrawMaterialTexture("Specular", material, Material::TextureType::Specular, m_Specular);
		DrawMaterialTexture("Normal", material, Material::TextureType::Normal, m_Normal);
		DrawMaterialTexture("Emissive", material, Material::TextureType::Emissive, m_Emissive);

		auto tiling = material.GetTextureTiling();
		if (GUIUtils::DrawVector2Control("Tiling", tiling, 1, 100))
			material.SetTextureTiling(tiling);

		auto offset = material.GetTextureOffset();
		if (GUIUtils::DrawVector2Control("Offset", offset, 0, 100))
			material.SetTextureOffset(offset);
	}
	void MaterialAsset::DrawMaterialTexture(const std::string& label, Material& material,
	                                        Material::TextureType textureType, Ref<TextureAsset>& textureAsset)
	{
		ImGui::PushID(static_cast<int>(textureType));

		if (GUIUtils::DrawTextureAsset(label, &textureAsset))
			material.SetTexture(textureType, textureAsset ? textureAsset->GetTexture() : nullptr);
		ImGui::PopID();
	}
}
