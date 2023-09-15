#pragma once
#include "DataTypes/Path.h"
#include "DataTypes/Utility.h"

namespace LevEngine
{
class Project
{
public:
    explicit Project(Path path) : m_Root(path.parent_path()), m_Path(Move(path)) { }
    
    static void Save();
    static bool Load(const Path& path);
    static const Path& GetRoot();
    static const Path& GetStartScene();
    [[nodiscard]] static const Ref<Project>& GetProject() { return s_Project; }
    static bool CreateNew(const Path& path);
    static void SetStartScene(Path path);
    [[nodiscard]] static Path GetPath();

private:
    Path m_Root;
    Path m_Path;
    String m_Name;
    Path m_StartScene;
    
    inline static Ref<Project> s_Project;
};    
}
