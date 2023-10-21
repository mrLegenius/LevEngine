#include "levpch.h"
#include "Project.h"

#include "ResourceManager.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
void Project::Save()
{
    LEV_CORE_ASSERT(s_Project, "No loaded project");

    const auto& project = s_Project;
    
    YAML::Emitter out;
    out << YAML::BeginMap;
    
    out << YAML::Key << "Project" << YAML::Value << project->m_Name.c_str();
    out << YAML::Key << "StartScene" << YAML::Value << project->m_StartScene.string().c_str();

    out << YAML::EndMap;

    std::ofstream fout(project->m_Path);
    fout << out.c_str();
}

bool Project::Load(const Path& path)
{
    if (path.empty()) return false;
    
    s_Project = CreateRef<Project>(path);
    YAML::Node data;
    if (!LoadYAMLFileSafe(path, data))
    {
        Log::CoreError("Failed to load project", path.string());
        return false;
    }

    if (!data["Project"]) return false;

    s_Project->m_Name = data["Project"].as<String>();
    if (const auto scene = data["StartScene"])
        s_Project->m_StartScene = scene.as<String>().c_str();

    return true;
}

const Path& Project::GetRoot()
{
    LEV_CORE_ASSERT(s_Project, "No loaded project");

    return s_Project->m_Root;
}

Path Project::GetStartScene()
{
    LEV_CORE_ASSERT(s_Project, "No loaded project");

    return s_Project->m_Root / s_Project->m_StartScene;
}

bool Project::CreateNew(const Path& path)
{
    if (path.empty()) return false;
    
    try
    {
        s_Project = CreateRef<Project>(path);
        s_Project->m_Name = path.stem().string().c_str();
        s_Project->Save();
    }
    catch (std::exception& e)
    {
        Log::CoreError("Failed to create new project in {0}. Error: {1}", path.string(), e.what());
        return false;
    }
        
    return true;
}

void Project::SetStartScene(const Path& path)
{
    LEV_CORE_ASSERT(s_Project, "No loaded project");

    s_Project->m_StartScene = relative(path, s_Project->m_Root);
}

Path Project::GetPath()
{
    LEV_CORE_ASSERT(s_Project, "No loaded project");
    return s_Project->m_Path;
}

void Project::Build()
{
    LEV_CORE_ASSERT(s_Project, "No loaded project");
    
    s_Project->CopyEngineResourceDirectory();
    ResourceManager::Build(s_Project->GetRoot());
}
    
void Project::CopyEngineResourceDirectory() const noexcept
{
    CopyRecursively(EngineResourcesRoot, m_Root / EngineResourcesRoot);
}
}

