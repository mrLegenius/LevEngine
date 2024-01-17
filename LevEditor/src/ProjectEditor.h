#pragma once

namespace LevEngine::Editor
{
    class MenuBar;

    class ProjectEditor
    {
    public:
        explicit ProjectEditor(const Action<>& onProjectOpened);
        
        void ShowProjectSelectionPopup();
        bool OpenProject();
        bool NewProject();
        static void SetCurrentSceneAsStartScene();
        void Update();

        void AddMainMenuItems(const Ref<MenuBar>& menuBar);

    private:
        Action<> m_ProjectOpened;
        bool OpenProjectRequested;
        bool CreateNewProjectRequested;
    };
}

