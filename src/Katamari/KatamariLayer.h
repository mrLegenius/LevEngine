#pragma once
#include "../Kernel/Layer.h"
#include "../OrbitCamera.h"
#include "Events/ApplicationEvent.h"
#include "Panels/ViewportPanel.h"
#include "Renderer/Texture.h"
#include "Scene/Scene.h"

class KatamariLayer final : public Layer
{
public:
	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void DrawToolbar();
	void DrawStatusbar();
	void DrawDockSpace();
	void OnEvent(Event& event) override;
	bool OnWindowResized(const WindowResizedEvent& e) const;
	void OnGUIRender() override;

private:
	Ref<Scene> m_Scene;

	Ref<Editor::ViewportPanel> m_Viewport;

	enum class SceneState
	{
		Edit = 0, Play = 1
	};
	SceneState m_SceneState = SceneState::Edit;

	Ref<Texture> m_IconPlay, m_IconStop;
};

