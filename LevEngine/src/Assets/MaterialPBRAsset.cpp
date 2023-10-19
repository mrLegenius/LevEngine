#include "levpch.h"
#include "MaterialPBRAsset.h"

#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
    void MaterialPBRAsset::SerializeData(YAML::Emitter& out)
    {
        out << YAML::Key << "Tint" << YAML::Value << m_Material.GetTintColor();
        out << YAML::Key << "Metallic" << YAML::Value << m_Material.GetMetallic();
        out << YAML::Key << "Roughness" << YAML::Value << m_Material.GetRoughness();
        out << YAML::Key << "EnableTransparency" << YAML::Value << m_Material.GetEnableTransparency();

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
        if (const auto transparency = node["EnableTransparency"])
            m_Material.SetEnableTransparency(transparency.as<bool>());

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

        if (m_Emissive)
            m_Material.SetTexture(MaterialPBR::TextureType::Emissive, m_Emissive->GetTexture());

        m_Material.SetTextureTiling(node["Tiling"].as<Vector2>());
        m_Material.SetTextureOffset(node["Offset"].as<Vector2>());
    }
}
