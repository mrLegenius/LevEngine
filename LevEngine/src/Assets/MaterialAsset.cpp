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

		for (int i = 0; i <= static_cast<int>(Material::TextureType::Normal); ++i)
		{
			const auto type = static_cast<Material::TextureType>(i);
			if (material.GetTexture(type))
				out << YAML::Key << "Texture" + std::to_string(i) << YAML::Value << material.GetTexture(type)->GetPath();
			out << YAML::Key << "Tiling" + std::to_string(i) << YAML::Value << material.GetTextureTiling(type);
			out << YAML::Key << "Offset" + std::to_string(i) << YAML::Value << material.GetTextureOffset(type);
		}
	}
	bool MaterialAsset::DeserializeData(YAML::Node& node)
	{
		material = {};

		material.SetAmbientColor(node["AmbientColor"].as<Color>());
		material.SetDiffuseColor(node["DiffuseColor"].as<Color>());
		material.SetSpecularColor(node["SpecularColor"].as<Color>());
		material.SetEmissiveColor(node["EmissiveColor"].as<Color>());
		material.SetShininess(node["Shininess"].as<float>());

		for (int i = 0; i <= static_cast<int>(Material::TextureType::Normal); ++i)
		{
			const auto type = static_cast<Material::TextureType>(i);
			if (node["Texture" + std::to_string(i)])
			{
				auto texturePathString = node["Texture" + std::to_string(i)].as<std::string>();
				material.SetTexture(type, TextureLibrary::GetTexture(texturePathString));
			}
			material.SetTextureTiling(type, node["Tiling" + std::to_string(i)].as<Vector2>());
			material.SetTextureOffset(type, node["Offset" + std::to_string(i)].as<Vector2>());
		}

		return true;
	}

	void MaterialAsset::DrawProperties()
	{
		GUIUtils::DrawColor3Control("Ambient", BindGetter(&Material::GetAmbientColor, &material), BindSetter(&Material::SetAmbientColor, &material));
		GUIUtils::DrawColor3Control("Diffuse", BindGetter(&Material::GetDiffuseColor, &material), BindSetter(&Material::SetDiffuseColor, &material));
		GUIUtils::DrawColor3Control("Specular", BindGetter(&Material::GetSpecularColor, &material), BindSetter(&Material::SetSpecularColor, &material));
		GUIUtils::DrawColor3Control("Emissive", BindGetter(&Material::GetEmissiveColor, &material), BindSetter(&Material::SetEmissiveColor, &material));

		GUIUtils::DrawFloatControl("Shininess", BindGetter(&Material::GetShininess, &material), BindSetter(&Material::SetShininess, &material), 1.0f, 1.0f, 128.0f);

		DrawMaterialTexture("Ambient", material, Material::TextureType::Ambient);
		DrawMaterialTexture("Diffuse", material, Material::TextureType::Diffuse);
		DrawMaterialTexture("Specular", material, Material::TextureType::Specular);
		DrawMaterialTexture("Normal", material, Material::TextureType::Normal);
		DrawMaterialTexture("Emissive", material, Material::TextureType::Emissive);

		if (ImGui::Button("Save"))
			Serialize();
	}
	void MaterialAsset::DrawMaterialTexture(const std::string& label, Material& material,
	                                        Material::TextureType textureType)
	{
		ImGui::PushID(static_cast<int>(textureType));
		ImGui::BeginTable("##texture_props", 2);

		ImGui::TableNextColumn();
		ImGui::BeginGroup();
		{
			ImGui::Text(label.c_str());

			auto tiling = material.GetTextureTiling(textureType);
			if (GUIUtils::DrawVector2Control("Tiling", tiling, 1))
				material.SetTextureTiling(textureType, tiling);

			auto offset = material.GetTextureOffset(textureType);
			if (GUIUtils::DrawVector2Control("Offset", offset))
				material.SetTextureOffset(textureType, offset);
		}
		ImGui::EndGroup();
		const ImVec2 size = ImGui::GetItemRectSize();

		ImGui::TableNextColumn();

		GUIUtils::DrawTexture2D([&material, textureType] {return material.GetTexture(textureType); },
		                        [&material, textureType](const Ref<Texture>& texture) { material.SetTexture(textureType, texture); }, Vector2(size.y, size.y));

		ImGui::EndTable();
		ImGui::PopID();
	}
}
