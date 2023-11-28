#pragma once

namespace LevEngine::Editor
{
    class EditorSaveData
    {
    public:
        explicit EditorSaveData(Path path);

        [[nodiscard]] Path GetLastOpenedProject() const { return m_LastOpenProjectPath; }
        void SetLastOpenedProject(const Path& projectPath) { m_LastOpenProjectPath = projectPath; }

        void Save() const;
        bool Load();
        
    private:
        Path m_Path;
        Path m_LastOpenProjectPath;
    };
}
