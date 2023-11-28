#include "pch.h"
#include "EditorSaveData.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine::Editor
{
    EditorSaveData::EditorSaveData(Path path): m_Path(Move(path))
    {
    }

    void EditorSaveData::Save() const
    {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "LastProjectPath" << YAML::Value << m_LastOpenProjectPath.string().c_str();

        out << YAML::EndMap;

        std::ofstream fout(m_Path);
        fout << out.c_str();
    }

    bool EditorSaveData::Load()
    {
        if (!exists(m_Path)) return false;

        YAML::Node data;
        if (!LoadYAMLFileSafe(m_Path, data))
        {
            Log::CoreError("Failed to load editor saved data");
            return false;
        }

        m_LastOpenProjectPath = data["LastProjectPath"].as<String>().c_str();
        return true;
    }
}
