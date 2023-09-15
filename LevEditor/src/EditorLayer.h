#pragma once
#include "EditorSaveData.h"
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
	void OpenProject();
	void NewProject();
	void LoadProject();
	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnScenePlay();
	void OnSceneStop();
	void DrawToolbar();
	void DrawStatusbar();
	void SetCurrentSceneAsStartScene() const;
	void DrawDockSpace();
	void CreateNewScene();
	void OpenScene();
	void OpenScene(const Path& path);
	bool SaveScene();
	bool SaveSceneAs();
	void OnEvent(Event& event) override;
	void OnDuplicateEntity() const;
	bool OnKeyPressed(KeyPressedEvent& event);
	bool OnWindowResized(const WindowResizedEvent& e) const;
	void OnGUIRender() override;

private:
	Ref<ViewportPanel> m_Viewport;
	Ref<HierarchyPanel> m_Hierarchy;
	Ref<PropertiesPanel> m_Properties;
	Ref<AssetBrowserPanel> m_AssetsBrowser;
	Ref<GamePanel> m_Game;
	std::shared_ptr<ConsolePanel> m_Console;

	EditorSaveData m_SaveData{"SaveData.editor"};
	
	enum class SceneState
	{
		Edit = 0, Play = 1
	};
	SceneState m_SceneState = SceneState::Edit;
};
}