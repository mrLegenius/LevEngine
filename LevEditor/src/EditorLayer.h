#pragma once
#include "EditorSaveData.h"
#include "ProjectEditor.h"
#include "SceneEditor.h"
#include "SceneState.h"

namespace LevEngine::Editor
{
    class AgentBridge;
    class StatusBar;
    class Toolbar;
    class TitleBar;
    class MenuBar;
    class DockSpace;
    class PanelManager;

    class EditorLayer final : public Layer
    {
    public:
        //<--- Defined where AgentBridge is a complete type, see the Scope member below ---<<
        EditorLayer();
        ~EditorLayer() override;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnScenePlay();
        void OnSceneStop();
        void OnEvent(Event& event) override;
        bool OnKeyPressed(KeyPressedEvent& event);
        void OnGUIRender() override;

        //<--- Used by the agent bridge, which drives the editor the way the buttons do ---<<
        [[nodiscard]] SceneState GetSceneState() const { return m_SceneState; }
        [[nodiscard]] const Ref<PanelManager>& GetPanelManager() const { return m_PanelManager; }
        [[nodiscard]] SceneEditor& GetSceneEditor() const { return *m_SceneEditor; }

        // Playing without a hand on the keyboard. Stepping a fixed number of frames is what makes a
        // measurement repeatable: wall clock time is not the same twice, a frame count is.
        [[nodiscard]] bool IsPaused() const { return m_IsPaused; }
        void SetPaused(const bool paused) { m_IsPaused = paused; m_StepsRemaining = 0; }
        void RequestSteps(const int frames) { m_StepsRemaining = frames; }

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

        bool m_IsPaused = false;
        int m_StepsRemaining = 0;

        Scope<AgentBridge> m_Agent;
    };
}
