#include "pch.h"
#include "SceneEditor.h"

#include "EntitySelection.h"
#include "SceneState.h"
#include "Essentials/MenuBar.h"

namespace LevEngine::Editor
{
    SceneEditor::SceneEditor(const Func<SceneState>& sceneStateGetter) : m_SceneStateGetter(sceneStateGetter)
    {
        
    }

    void SceneEditor::CreateNewScene()
    {
        SceneManager::LoadEmptyScene();
    }

    void SceneEditor::OpenScene() const
    {
        const auto filepath = FileDialogs::OpenFile("LevEngine Scene (*.scene)\0*.scene\0");
        if (!filepath.empty())
        {
            auto _ = OpenScene(filepath.c_str());
        }
    }

    bool SceneEditor::OpenScene(const Path& path) const
    {
        if (path.extension().string() != ".scene")
        {
            Log::Warning("Failed to open scene. {0} is not a scene file", path.filename().string());
            return false;
        }

        if (m_SceneStateGetter() != SceneState::Edit)
        {
            Log::CoreWarning("Can't open scene in play mode");
            return false;
        }
        
        if (SceneManager::LoadScene(path))
        {
            Selection::Deselect();
            return true;
        }

        return false;
    }

    bool SceneEditor::SaveScene() const
    {
        if (m_SceneStateGetter() == SceneState::Play)
        {
            Log::CoreWarning("Can't save scene in play mode");
            return false;
        }

        if (const auto scenePath = SceneManager::GetActiveScenePath(); !scenePath.empty())
        {
            SceneManager::SaveScene(ToString(scenePath));
            return true;
        }

        return SaveSceneAs();
    }

    bool SceneEditor::SaveSceneAs() const
    {
        if (m_SceneStateGetter() == SceneState::Play)
        {
            Log::CoreWarning("Can't save scene in play mode");
            return false;
        }

        const auto filepath = FileDialogs::SaveFile("LevEngine Scene (*.scene)\0*.scene\0", "scene");
        if (!filepath.empty())
        {
            SceneManager::SaveScene(filepath);
            return true;
        }

        return false;
    }

    bool SceneEditor::OnKeyPressed(const KeyPressedEvent& e) const
    {
        //Shortcuts
        if (e.GetRepeatCount() > 0)
            return false;

        const bool control = Input::IsKeyDown(KeyCode::LeftControl) ||
            Input::IsKeyDown(KeyCode::RightControl);
        const bool shift = Input::IsKeyDown(KeyCode::LeftShift) ||
            Input::IsKeyDown(KeyCode::RightShift);

        switch (e.GetKeyCode())
        {

        case KeyCode::N:
            {
                if (control) { CreateNewScene(); }
                break;
            }
        case KeyCode::O:
            {
                if (control) { OpenScene(); }
                break;
            }
        case KeyCode::S:
            {
                if (control && shift) { auto _ = SaveSceneAs(); }
                else if (control) { auto _ = SaveScene(); }
                break;
            }
        case KeyCode::D:
            {
                if (control)
                    OnDuplicateEntity();

                break;
            }

        default:
            break;
        }

        return false;
    }

    void SceneEditor::OnDuplicateEntity()
    {
        if (const auto entitySelection = Selection::CurrentAs<EntitySelection>())
        {
            SceneManager::GetActiveScene()->DuplicateEntity(entitySelection->Get());
        }
    }

    void SceneEditor::AddMainMenuItems(const Ref<MenuBar>& menuBar) const
    {
        menuBar->AddMenuItem("File/New Scene", "Ctrl+N", [this] { CreateNewScene(); });
        menuBar->AddMenuItem("File/Open Scene...", "Ctrl+O", [this] { OpenScene(); });
        menuBar->AddMenuItem("File/Save Scene", "Ctrl+S", [this] { SaveScene(); });
        menuBar->AddMenuItem("File/Save Scene As...", "Ctrl+Shift+S", [this] { SaveSceneAs(); });
    }
}
