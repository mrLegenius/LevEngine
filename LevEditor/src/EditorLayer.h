#pragma once
#include "EditorSaveData.h"
#include "ProjectEditor.h"
#include "SceneEditor.h"
#include "SceneState.h"

namespace LevEngine::Editor
{
    class StatusBar;
    class Toolbar;
    class MenuBar;
    class ConsolePanel;
    class SettingsPanel;
    class GamePanel;
    class AssetBrowserPanel;
    class PropertiesPanel;
    class HierarchyPanel;
    class ViewportPanel;
    class DockSpace;
    class ScriptsPanel;
    class StatisticsPanel;
    
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
        void OnProjectLoaded();
        void OnPlayButtonClicked();

        Scope<ProjectEditor> m_ProjectEditor;
        Scope<SceneEditor> m_SceneEditor;

        Ref<DockSpace> m_DockSpace;
        Ref<ViewportPanel> m_Viewport;
        Ref<HierarchyPanel> m_Hierarchy;
        Ref<PropertiesPanel> m_Properties;
        Ref<AssetBrowserPanel> m_AssetsBrowser;
        Ref<GamePanel> m_Game;
        Ref<SettingsPanel> m_Settings;
        std::shared_ptr<ConsolePanel> m_Console;
        Ref<MenuBar> m_MainMenuBar;
        Ref<Toolbar> m_MainToolbar;
        Ref<StatusBar> m_MainStatusBar;
        Ref<StatisticsPanel> m_Statistics;
        Ref<ScriptsPanel> m_ScriptsPanel;

        EditorSaveData m_SaveData{"SaveData.editor"};
        
        SceneState m_SceneState = SceneState::Edit;
    };
}
