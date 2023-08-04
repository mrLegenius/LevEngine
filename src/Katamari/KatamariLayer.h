#pragma once
#include "../Kernel/Layer.h"
#include "../OrbitCamera.h"
#include "Scene/Scene.h"

class KatamariLayer final : public Layer
{
public:
	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void DrawToolbar();
	void DrawDockSpace();
	void DrawViewport();
	void OnEvent(Event& event) override;
	void OnGUIRender() override;

private:
	Ref<Scene> m_Scene;

	bool m_ViewportFocused = false;
	bool m_ViewportHovered = false;
	Vector2 m_ViewportSize{ 0.0f };
	Vector2 m_ViewportBounds[2];
};

