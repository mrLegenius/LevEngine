#pragma once
#include "../Kernel/Layer.h"
#include "../OrbitCamera.h"
#include "../Renderer/D3D11ConstantBuffer.h"
#include "Scene/Scene.h"

class KatamariLayer final : public Layer
{
public:
	void OnAttach() override;
	void OnUpdate(float deltaTime) override;

private:
	Ref<Scene> m_Scene;
	std::shared_ptr<D3D11ConstantBuffer> m_CameraConstantBuffer;
	std::shared_ptr<D3D11ConstantBuffer> m_DirLightConstantBuffer;
};

