#pragma once
#include "EditorSaveData.h"
#include "Essentials/MenuBar.h"
#include "Events/ApplicationEvent.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/GamePanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/SettingsPanel.h"
#include "Panels/ViewportPanel.h"

namespace LevEngine::Editor
{
    class EditorLayer final : public Layer
    {
    public:
        void ShowProjectSelectionPopup();
        bool OpenProject();
        bool NewProject();
        void LoadProject();
        void OnAttach() override;
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnScenePlay();
        void OnSceneStop();
        void DrawToolbar();
        void DrawStatusbar();
        void SetCurrentSceneAsStartScene() const;
        void DrawDockSpace();
        void CreateNewScene();
        void OpenScene();
        bool OpenScene(const Path& path);
        bool SaveScene();
        bool SaveSceneAs();
        void OnEvent(Event& event) override;
        static void OnDuplicateEntity();
        bool OnKeyPressed(KeyPressedEvent& event);
        bool OnWindowResized(const WindowResizedEvent& e) const;
        void OnGUIRender() override;

    private:
        static void DoComponentRenderDebug();

        Ref<ViewportPanel> m_Viewport;
        Ref<HierarchyPanel> m_Hierarchy;
        Ref<PropertiesPanel> m_Properties;
        Ref<AssetBrowserPanel> m_AssetsBrowser;
        Ref<GamePanel> m_Game;
        Ref<SettingsPanel> m_Settings;
        std::shared_ptr<ConsolePanel> m_Console;
        Ref<MenuBar> m_MainMenuBar;

        EditorSaveData m_SaveData{"SaveData.editor"};

        enum class SceneState
        {
            Edit = 0,
            Play = 1
        };

        SceneState m_SceneState = SceneState::Edit;
    };
}
