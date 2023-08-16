#pragma once
#include "EntitySelection.h"
#include "../Kernel/Layer.h"
#include "../OrbitCamera.h"
#include "Events/ApplicationEvent.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/ViewportPanel.h"
#include "Renderer/Texture.h"
#include "Scene/Scene.h"

class KatamariLayer final : public Layer
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
	bool OnWindowResized(const WindowResizedEvent& e) const;
	void OnGUIRender() override;

private:
	Ref<Scene> m_ActiveScene;

	Ref<Editor::ViewportPanel> m_Viewport;
	Ref<Editor::HierarchyPanel> m_Hierarchy;
	Ref<Editor::PropertiesPanel> m_Properties;
	Ref<Editor::AssetBrowserPanel> m_AssetsBrowser;
	std::filesystem::path m_EditorScenePath;

	enum class SceneState
	{
		Edit = 0, Play = 1
	};
	SceneState m_SceneState = SceneState::Edit;

	Ref<Texture> m_IconPlay, m_IconStop;
};

