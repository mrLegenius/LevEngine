#include "levpch.h"
#include "MaterialCustomAsset.h"

#include "ShaderAsset.h"
#include "TextureAsset.h"
#include "Renderer/Pipeline/Texture.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
    void MaterialCustomAsset::SetShaderAsset(const Ref<ShaderAsset>& shader)
    {
        m_Shader = shader;
        m_Material.SetShaderPath(shader ? shader->GetPath() : Path{});
    }

    Ref<TextureAsset> MaterialCustomAsset::GetTexture(const String& name) const
    {
        const auto it = m_Textures.find(name);
        return it != m_Textures.end() ? it->second : nullptr;
    }

    void MaterialCustomAsset::SetTexture(const String& name, const Ref<TextureAsset>& texture)
    {
        if (texture)
            m_Textures[name] = texture;
        else
            m_Textures.erase(name);

        m_Material.SetTexture(name, texture ? texture->GetTexture() : nullptr);
    }

    void MaterialCustomAsset::SerializeData(YAML::Emitter& out)
    {
        SerializeAsset(out, "Shader", m_Shader);

        out << YAML::Key << "Transparent" << YAML::Value << m_Material.IsTransparent();

        out << YAML::Key << "Properties" << YAML::Value << YAML::BeginSeq;
        for (const auto& [name, value] : m_Material.GetPropertyValues())
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << name;
            out << YAML::Key << "Type" << YAML::Value << static_cast<int>(value.Type);

            switch (value.Type)
            {
            case ShaderPropertyType::Float:
                out << YAML::Key << "Value" << YAML::Value << value.Float[0];
                break;
            case ShaderPropertyType::Float2:
                out << YAML::Key << "Value" << YAML::Value << Vector2{value.Float[0], value.Float[1]};
                break;
            case ShaderPropertyType::Float3:
                out << YAML::Key << "Value" << YAML::Value << Vector3{value.Float[0], value.Float[1], value.Float[2]};
                break;
            case ShaderPropertyType::Float4:
                out << YAML::Key << "Value" << YAML::Value
                    << Vector4{value.Float[0], value.Float[1], value.Float[2], value.Float[3]};
                break;
            case ShaderPropertyType::Int:
            case ShaderPropertyType::Bool:
                out << YAML::Key << "Value" << YAML::Value << value.Int[0];
                break;
            default:
                break;
            }

            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;
        for (const auto& [name, texture] : m_Textures)
        {
            if (!texture) continue;

            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << name;
            SerializeAsset(out, "Texture", texture);
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
    }

    void MaterialCustomAsset::DeserializeData(const YAML::Node& node)
    {
        m_Material = {};
        m_Textures.clear();

        SetShaderAsset(DeserializeAsset<ShaderAsset>(node["Shader"]));

        bool isTransparent = false;
        TryParse(node["Transparent"], isTransparent);
        m_Material.SetTransparent(isTransparent);

        if (const auto properties = node["Properties"])
        {
            for (const auto property : properties)
            {
                String name;
                if (!TryParse(property["Name"], name)) continue;

                int type = 0;
                if (!TryParse(property["Type"], type)) continue;

                const auto valueNode = property["Value"];
                if (!valueNode) continue;

                switch (static_cast<ShaderPropertyType>(type))
                {
                case ShaderPropertyType::Float:
                    m_Material.SetFloat(name, valueNode.as<float>());
                    break;
                case ShaderPropertyType::Float2:
                    m_Material.SetVector2(name, valueNode.as<Vector2>());
                    break;
                case ShaderPropertyType::Float3:
                    m_Material.SetVector3(name, valueNode.as<Vector3>());
                    break;
                case ShaderPropertyType::Float4:
                    m_Material.SetVector4(name, valueNode.as<Vector4>());
                    break;
                case ShaderPropertyType::Int:
                    m_Material.SetInt(name, valueNode.as<int>());
                    break;
                case ShaderPropertyType::Bool:
                    m_Material.SetBool(name, valueNode.as<int>() != 0);
                    break;
                default:
                    break;
                }
            }
        }

        if (const auto textures = node["Textures"])
        {
            for (const auto texture : textures)
            {
                String name;
                if (!TryParse(texture["Name"], name)) continue;

                SetTexture(name, DeserializeAsset<TextureAsset>(texture["Texture"]));
            }
        }
    }
}
