#pragma once

class EditorSaveData
{
public:
    explicit EditorSaveData(Path path) : m_Path(Move(path)) { }
    
    void SetLastOpenedProject(const Path& projectPath)
    {
        m_LastOpenProjectPath = projectPath;
    }
    
    void Save() const
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
    
        out << YAML::Key << "LastProjectPath" << YAML::Value << m_LastOpenProjectPath.string().c_str();

        out << YAML::EndMap;

        std::ofstream fout(m_Path);
        fout << out.c_str();
    }
    
    bool Load()
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(m_Path.string());
        }
        catch (std::exception& e)
        {
            Log::CoreError("Failed to load saved data in {0}. Error: {1}", m_Path.string(), e.what());
            return false;
        }

        m_LastOpenProjectPath = data["LastProjectPath"].as<String>().c_str();
        return true;
    }

    [[nodiscard]] Path GetLastOpenedProject() const { return m_LastOpenProjectPath; }

private:
    Path m_Path;
    Path m_LastOpenProjectPath;
};
