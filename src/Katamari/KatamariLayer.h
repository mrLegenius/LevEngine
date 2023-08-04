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
	void OnEvent(Event& event) override;
	void OnGUIRender() override;

private:
	Ref<Scene> m_Scene;
};

