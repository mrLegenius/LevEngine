#pragma once
#include "../Kernel/Layer.h"
#include "../FreeCamera.h"
#include "../Renderer/D3D11ConstantBuffer.h"

class KatamariLayer final : public Layer
{
public:
	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnEvent(Event& event) override;

private:
	std::shared_ptr<SceneCamera> m_Camera;
	std::shared_ptr<D3D11ConstantBuffer> m_CameraConstantBuffer;
};

