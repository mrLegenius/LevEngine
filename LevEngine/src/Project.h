#pragma once
#include "Physics/PhysicsSettingsSerializer.h"
#include "Serializers/RenderSettingsSerializer.h"

namespace LevEngine
{
    class Project
    {
    public:
        explicit Project(Path path) : m_Root(path.parent_path()), m_Path(Move(path)) { }
        
        static void Save();
        static bool Load(const Path& path);
        static const Path& GetRoot();
        static Path GetStartScene();
        [[nodiscard]] static const Ref<Project>& GetProject() { return s_Project; }
        static bool CreateNew(const Path& path);
        static void SetStartScene(const Path& path);
        [[nodiscard]] static Path GetPath();
        static void Build();
        void CopyEngineResourceDirectory() const noexcept;

        static void SaveSettings();

    private:
        Path m_Root;
        Path m_Path;
        String m_Name;
        Path m_StartScene;

        void LoadSettings();

        inline static Ref<Project> s_Project;

        RenderSettingsSerializer m_RenderSettingsSerializer;
        PhysicsSettingsSerializer m_PhysicsSettingsSerializer;
    };    
}
