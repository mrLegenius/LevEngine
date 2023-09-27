#include "levpch.h"
#include "MaterialPBRAsset.h"

#include "GUI/GUIUtils.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
    void MaterialPBRAsset::DrawProperties()
    {
        GUIUtils::DrawColor3Control("Tint", BindGetter(&MaterialPBR::GetTintColor, &m_Material), BindSetter(&MaterialPBR::SetTintColor, &m_Material));

        GUIUtils::DrawFloatControl("Metallic", BindGetter(&MaterialPBR::GetMetallic, &m_Material), BindSetter(&MaterialPBR::SetMetallic, &m_Material), 0.01f, 0.04f, 1.0f);
        GUIUtils::DrawFloatControl("Roughness", BindGetter(&MaterialPBR::GetRoughness, &m_Material), BindSetter(&MaterialPBR::SetRoughness, &m_Material), 0.01f, 0.0f, 1.0f);

        DrawMaterialTexture("Albedo", m_Material, MaterialPBR::TextureType::Albedo, m_Albedo);
        DrawMaterialTexture("Metallic", m_Material, MaterialPBR::TextureType::Metallic, m_Metallic);
        DrawMaterialTexture("Roughness", m_Material, MaterialPBR::TextureType::Roughness, m_Roughness);
        DrawMaterialTexture("Normal", m_Material, MaterialPBR::TextureType::Normal, m_Normal);
        DrawMaterialTexture("AmbientOcclusion", m_Material, MaterialPBR::TextureType::AmbientOcclusion, m_AmbientOcclusion);
        DrawMaterialTexture("Emissive", m_Material, MaterialPBR::TextureType::Emissive, m_Emissive);

        auto tiling = m_Material.GetTextureTiling();
        if (GUIUtils::DrawVector2Control("Tiling", tiling, 1, 100))
            m_Material.SetTextureTiling(tiling);

        auto offset = m_Material.GetTextureOffset();
        if (GUIUtils::DrawVector2Control("Offset", offset, 0, 100))
            m_Material.SetTextureOffset(offset);
    }

    void MaterialPBRAsset::SerializeData(YAML::Emitter& out)
    {
        out << YAML::Key << "Tint" << YAML::Value << m_Material.GetTintColor();
        out << YAML::Key << "Metallic" << YAML::Value << m_Material.GetMetallic();
        out << YAML::Key << "Roughness" << YAML::Value << m_Material.GetRoughness();

        SerializeAsset(out, "Albedo", m_Albedo);
        SerializeAsset(out, "MetallicTexture", m_Metallic);
        SerializeAsset(out, "RoughnessTexture", m_Roughness);
        SerializeAsset(out, "AmbientOcclusion", m_AmbientOcclusion);
        SerializeAsset(out, "Normal", m_Normal);
        SerializeAsset(out, "Emissive", m_Emissive);

        out << YAML::Key << "Tiling" << YAML::Value << m_Material.GetTextureTiling();
        out << YAML::Key << "Offset" << YAML::Value << m_Material.GetTextureOffset();
    }

    void MaterialPBRAsset::DeserializeData(YAML::Node& node)
    {
        m_Material = {};

        m_Material.SetTintColor(node["Tint"].as<Color>());
        m_Material.SetMetallic(node["Metallic"].as<float>());
        m_Material.SetRoughness(node["Roughness"].as<float>());

        m_Albedo = DeserializeAsset<TextureAsset>(node["Albedo"]);
        m_Metallic = DeserializeAsset<TextureAsset>(node["MetallicTexture"]);
        m_Roughness = DeserializeAsset<TextureAsset>(node["RoughnessTexture"]);
        m_AmbientOcclusion = DeserializeAsset<TextureAsset>(node["AmbientOcclusion"]);
        m_Normal = DeserializeAsset<TextureAsset>(node["Normal"]);
        m_Emissive = DeserializeAsset<TextureAsset>(node["Emissive"]);

        if (m_Albedo)
            m_Material.SetTexture(MaterialPBR::TextureType::Albedo, m_Albedo->GetTexture());

        if (m_Metallic)
            m_Material.SetTexture(MaterialPBR::TextureType::Metallic, m_Metallic->GetTexture());

        if (m_Roughness)
            m_Material.SetTexture(MaterialPBR::TextureType::Roughness, m_Roughness->GetTexture());

        if (m_AmbientOcclusion)
            m_Material.SetTexture(MaterialPBR::TextureType::AmbientOcclusion, m_AmbientOcclusion->GetTexture());

        if (m_Normal)
            m_Material.SetTexture(MaterialPBR::TextureType::Normal, m_Normal->GetTexture());

        m_Material.SetTextureTiling(node["Tiling"].as<Vector2>());
        m_Material.SetTextureOffset(node["Offset"].as<Vector2>());
    }

    void MaterialPBRAsset::DrawMaterialTexture(const String& label, MaterialPBR& MaterialPBR,
        MaterialPBR::TextureType textureType, Ref<TextureAsset>& textureAsset)
    {
        ImGui::PushID(static_cast<int>(textureType));

        if (GUIUtils::DrawTextureAsset(label, textureAsset))
            MaterialPBR.SetTexture(textureType, textureAsset ? textureAsset->GetTexture() : nullptr);
        ImGui::PopID();
    }
}
