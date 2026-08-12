#pragma once
#include "EditorSaveData.h"
#include "ProjectEditor.h"
#include "SceneEditor.h"
#include "SceneState.h"

namespace LevEngine::Editor
{
    class StatusBar;
    class Toolbar;
    class TitleBar;
    class MenuBar;
    class DockSpace;
    class PanelManager;

    class EditorLayer final : public Layer
    {
    public:
        void OnAttach() override;
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnScenePlay();
        void OnSceneStop();
        void OnEvent(Event& event) override;
        bool OnKeyPressed(KeyPressedEvent& event);
        void OnGUIRender() override;

    private:
        static void DoComponentRenderDebug();
        [[nodiscard]] static String GetWindowTitle();
        void RegisterPanels();
        void FocusViewportsOn(Entity entity) const;
        void OnProjectLoaded();
        void OnPlayButtonClicked();

        Scope<ProjectEditor> m_ProjectEditor;
        Scope<SceneEditor> m_SceneEditor;

        Ref<DockSpace> m_DockSpace;
        Ref<PanelManager> m_PanelManager;
        Ref<MenuBar> m_MainMenuBar;
        Ref<TitleBar> m_MainTitleBar;
        Ref<Toolbar> m_MainToolbar;
        Ref<StatusBar> m_MainStatusBar;

        EditorSaveData m_SaveData{"SaveData.editor"};

        SceneState m_SceneState = SceneState::Edit;
    };
}
