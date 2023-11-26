#include "pch.h"
#include "ProjectEditor.h"

#include "ModalPopup.h"
#include "Project.h"
#include "Essentials/MenuBar.h"

namespace LevEngine::Editor
{
    ProjectEditor::ProjectEditor(const Action<>& onProjectOpened) : m_ProjectOpened(onProjectOpened)
    {
    }

    void ProjectEditor::ShowProjectSelectionPopup()
    {
        ModalPopup::Show(
            "Project Selection",
            "Open existing or create new project",
            "Open",
            "Create new",
            [this] { if (!OpenProject()) ShowProjectSelectionPopup(); },
            [this] { if (!NewProject()) ShowProjectSelectionPopup(); });
    }

    bool ProjectEditor::OpenProject()
    {
        const String& path = FileDialogs::OpenFile("LevProject (*.levproject)\0*.levproject\0");

        if(Project::Load(path.c_str()))
        {
            m_ProjectOpened();
            return true;
        }

        return false;
    }

    bool ProjectEditor::NewProject()
    {
        const String& path = FileDialogs::SaveFile("LevProject (*.levproject)\0*.levproject\0", "levproject");

        if(Project::CreateNew(path.c_str()))
        {
            m_ProjectOpened();
            return true;
        }
        
        return false;
    }

    void ProjectEditor::SetCurrentSceneAsStartScene()
    {
        if (!SceneManager::GetActiveScene())
        {
            Log::CoreWarning("There is no active scene");
            return;
        }

        Project::SetStartScene(SceneManager::GetActiveScenePath());
        Project::Save();
    }

    void ProjectEditor::AddMainMenuItems(const Ref<MenuBar>& menuBar)
    {
        menuBar->AddMenuItem("Project/Open project...", String(), [this]
        {
            if (!OpenProject())
                Log::CoreWarning("Failed to open project");
        });
        menuBar->AddMenuItem("Project/Create New...", String(), [this]
        {
            if(!NewProject())
                Log::CoreWarning("Failed to open project");
        });
        menuBar->AddMenuItem("Project/Set current scene as start scene", String(), [this]
        {
            SetCurrentSceneAsStartScene();
        });
        menuBar->AddMenuItem("Project/Build", String(), [this]
        {
            Project::Build();
        });
    }
}
