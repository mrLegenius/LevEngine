#include "levpch.h"
#include "ScriptAsset.h"
#include "EngineAssets.h"

namespace LevEngine
{
    const Path DefaultSystemPath = EngineResourcesRoot / "Scripts/System.lua";
    const Path DefaultComponentPath = EngineResourcesRoot / "Scripts/Component.lua";
    
    ScriptAsset::ScriptAsset(const Path& path, const UUID uuid):
        Asset(path, uuid),
        m_Type(Type::Undefined)
    {
    }

    ScriptAsset::ScriptAsset(const Path& path, const UUID uuid, Type type):
        Asset(path, uuid),
        m_Type(type)
    {
        if (!exists(path))
        {
            switch (type) {
            case Type::System:
                copy(DefaultSystemPath, path, std::filesystem::copy_options::overwrite_existing);
                break;
            case Type::Component:
                copy(DefaultComponentPath, path, std::filesystem::copy_options::overwrite_existing);
                break;
            default:
                std::ofstream fout(m_Path);
            }
        }
    }

    ScriptAsset::Type ScriptAsset::GetType() const
    {
        return m_Type;
    }

    void ScriptAsset::SetType(Type type)
    {
        m_Type = type;
    }

    bool ScriptAsset::WriteDataToFile() const
    {
        return false;
    }

    bool ScriptAsset::ReadDataFromFile() const
    {
        return false;
    }

    void ScriptAsset::SerializeData(YAML::Emitter& out)
    {
    }

    void ScriptAsset::DeserializeData(YAML::Node& node)
    {
    }

    void ScriptAsset::SerializeMeta(YAML::Emitter& out)
    {
        out << YAML::Key << "Type" << YAML::Value << static_cast<int>(m_Type);
    }

    void ScriptAsset::DeserializeMeta(YAML::Node& node)
    {
        try
        {
            if (const auto address = node["Type"])
                m_Type = static_cast<Type>(address.as<int>());
        }
        catch ([[maybe_unused]] std::exception& e)
        {
            m_Type = Type::Undefined;
        }
    }
}
