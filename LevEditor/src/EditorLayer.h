#pragma once
#include "Events/ApplicationEvent.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/GamePanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/ViewportPanel.h"

namespace LevEngine::Editor
{
class EditorLayer final : public Layer
{
public:
	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnScenePlay();
	void OnSceneStop();
	void DrawToolbar();
	void DrawStatusbar();
	void DrawDockSpace();
	void CreateNewScene();
	void OpenScene();
	void OpenScene(const std::filesystem::path& path);
	bool SaveScene();
	bool SaveSceneAs();
	void OnEvent(Event& event) override;
	bool OnKeyPressed(KeyPressedEvent& event);
	bool OnWindowResized(const WindowResizedEvent& e) const;
	void OnGUIRender() override;

private:
	Ref<Scene> m_ActiveScene;

	Ref<ViewportPanel> m_Viewport;
	Ref<HierarchyPanel> m_Hierarchy;
	Ref<PropertiesPanel> m_Properties;
	Ref<AssetBrowserPanel> m_AssetsBrowser;
	Ref<GamePanel> m_Game;
	Ref<ConsolePanel> m_Console;

	std::filesystem::path m_EditorScenePath;

	enum class SceneState
	{
		Edit = 0, Play = 1
	};
	SceneState m_SceneState = SceneState::Edit;
};
}