#pragma once
#include "../Kernel/Layer.h"
#include "../Renderer/D3D11Shader.h"
#include "../Renderer/D3D11ConstantBuffer.h"
#include "FreeCamera.h"

class Mesh;
class Body;

class SolarSystemLayer : public Layer
{
public:
	SolarSystemLayer() : Layer("Solar System Layer") { }
	~SolarSystemLayer() override = default;

	void OnAttach() override;
	void OnUpdate() override;
	void OnEvent(Event& event) override;

private:

	std::shared_ptr<D3D11Shader> m_MeshShader;
	std::shared_ptr<FreeCamera> m_FreeCamera;
	std::shared_ptr<D3D11ConstantBuffer> m_CameraConstantBuffer;

	std::shared_ptr<Body> m_TestBody;
};

