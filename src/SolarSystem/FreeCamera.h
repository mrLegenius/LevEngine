#pragma once
#include "../Renderer/Camera/SceneCamera.h"


#include <SimpleMath.h>

class FreeCamera : public SceneCamera
{
public:
	FreeCamera() = default;
	FreeCamera(float fov, float nearClip, float farClip);

	void Update(float deltaTime);
private:

	DirectX::SimpleMath::Vector2 m_InitialMousePosition = { 0.0f, 0.0f };
};