#pragma once
#include "../Kernel/Layer.h"
#include "../Renderer/D3D11ConstantBuffer.h"
#include "../FreeCamera.h"
#include "../OrbitCamera.h"

class SkyboxRenderer;
class Mesh;
class Body;

class SolarSystemLayer : public Layer
{
public:
	SolarSystemLayer() : Layer("Solar System Layer") { }
	~SolarSystemLayer() override = default;

	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnEvent(Event& event) override;

private:
	std::shared_ptr<FreeCamera> m_FreeCamera;
	std::shared_ptr<OrbitCamera> m_OrbitCamera;
	std::shared_ptr<D3D11ConstantBuffer> m_CameraConstantBuffer;

	std::shared_ptr<Body> m_Sun;
	std::shared_ptr<Body> m_Mercury;
	std::shared_ptr<Body> m_Venus;
	std::shared_ptr<Body> m_Earth; std::shared_ptr<Body> m_Moon;
	std::shared_ptr<Body> m_Mars;
	std::shared_ptr<Body> m_Jupiter;
	std::shared_ptr<Body> m_Saturn;
	std::shared_ptr<Body> m_Uranus;
	std::shared_ptr<Body> m_Neptune;

	std::shared_ptr<SkyboxRenderer> m_Skybox;

	bool m_UseFreeCamera = true;
};

