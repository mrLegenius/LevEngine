#pragma once
#include "EditorSaveData.h"
#include "ProjectEditor.h"
#include "SceneEditor.h"
#include "SceneState.h"
#include "Essentials/MenuBar.h"
#include "Events/ApplicationEvent.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/DockSpace.h"
#include "Panels/GamePanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/SettingsPanel.h"
#include "Panels/StatusBar.h"
#include "Panels/Toolbar.h"
#include "Panels/ViewportPanel.h"

namespace LevEngine::Editor
{
    class EditorLayer final : public Layer
    {
    public:
        void OnAttach() override;
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnScenePlay();
        void OnSceneStop();
        void OnEvent(Event& event) override;
        bool OnKeyPressed(KeyPressedEvent& event) const;
        static bool OnWindowResized(const WindowResizedEvent& e);
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

        EditorSaveData m_SaveData{"SaveData.editor"};
        
        SceneState m_SceneState = SceneState::Edit;
        
    };
}
