#include "levpch.h"
#include "MaterialPBRAsset.h"

#include "EngineAssets.h"
#include "TextureAsset.h"
#include "Renderer/Pipeline/Texture.h"

#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
    MaterialPBRAsset::MaterialPBRAsset(const Path& path, const UUID uuid)
        : MaterialAsset(path, uuid)
    {
        ShaderAssets::ForwardPBR();
    }

    MaterialPBRAsset::MaterialPBRAsset(const Path& path, UUID uuid, const MaterialPBR& material)
        : MaterialAsset(path, uuid), m_Material(material)
    {
    }

    Ref<TextureAsset> MaterialPBRAsset::GetTexture(MaterialPBR::TextureType type)
    {
        auto it = m_Textures.find(type);
        if (it == m_Textures.end())
            return nullptr;

        return it->second;
    }

    void MaterialPBRAsset::SetTexture(MaterialPBR::TextureType type, const Ref<TextureAsset>& textureAsset)
    {
        m_Textures[type] = textureAsset;
        
        if (!textureAsset) return;
        
        m_Material.SetTexture(type, textureAsset->GetTexture());
    }

    void MaterialPBRAsset::SerializeData(YAML::Emitter& out)
    {
        out << YAML::Key << "Tint" << YAML::Value << m_Material.GetTintColor();
        out << YAML::Key << "Metallic" << YAML::Value << m_Material.GetMetallic();
        out << YAML::Key << "Roughness" << YAML::Value << m_Material.GetRoughness();
        out << YAML::Key << "EnableTransparency" << YAML::Value << m_Material.GetEnableTransparency();

        SerializeAsset(out, "Albedo", GetTexture(MaterialPBR::TextureType::Albedo));
        SerializeAsset(out, "MetallicTexture", GetTexture(MaterialPBR::TextureType::Metallic));
        SerializeAsset(out, "RoughnessTexture", GetTexture(MaterialPBR::TextureType::Roughness));
        SerializeAsset(out, "AmbientOcclusion", GetTexture(MaterialPBR::TextureType::AmbientOcclusion));
        SerializeAsset(out, "Normal", GetTexture(MaterialPBR::TextureType::Normal));
        SerializeAsset(out, "Emissive", GetTexture(MaterialPBR::TextureType::Emissive));

        out << YAML::Key << "Tiling" << YAML::Value << m_Material.GetTextureTiling();
        out << YAML::Key << "Offset" << YAML::Value << m_Material.GetTextureOffset();
    }

    void MaterialPBRAsset::DeserializeData(const YAML::Node& node)
    {
        m_Material = {};

        m_Material.SetTintColor(node["Tint"].as<Color>());
        m_Material.SetMetallic(node["Metallic"].as<float>());
        m_Material.SetRoughness(node["Roughness"].as<float>());
        if (const auto transparency = node["EnableTransparency"])
            m_Material.SetEnableTransparency(transparency.as<bool>());
        
        SetTexture(MaterialPBR::TextureType::Albedo, DeserializeAsset<TextureAsset>(node["Albedo"]));
        SetTexture(MaterialPBR::TextureType::Metallic, DeserializeAsset<TextureAsset>(node["MetallicTexture"]));
        SetTexture(MaterialPBR::TextureType::Roughness, DeserializeAsset<TextureAsset>(node["RoughnessTexture"]));
        SetTexture(MaterialPBR::TextureType::AmbientOcclusion, DeserializeAsset<TextureAsset>(node["AmbientOcclusion"]));
        SetTexture(MaterialPBR::TextureType::Normal, DeserializeAsset<TextureAsset>(node["Normal"]));
        SetTexture(MaterialPBR::TextureType::Emissive, DeserializeAsset<TextureAsset>(node["Emissive"]));

        m_Material.SetTextureTiling(node["Tiling"].as<Vector2>());
        m_Material.SetTextureOffset(node["Offset"].as<Vector2>());
    }
}
