#pragma once
#include "../Renderer/Camera/SceneCamera.h"
#include "../Events/Event.h"
#include "../Events/MouseEvent.h"
#include "../Components/Transform.h"

#include <SimpleMath.h>

class FreeCamera : public SceneCamera
{
public:
	FreeCamera() = default;
	FreeCamera(float fov, float nearClip, float farClip);

	void Update(float deltaTime);

	[[nodiscard]] const DirectX::SimpleMath::Matrix& GetViewMatrix() const { return m_ViewMatrix; }
	[[nodiscard]] DirectX::SimpleMath::Matrix GetViewProjection() const { return m_ViewMatrix * m_Projection; }

	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetPosition() const { return m_Transform.position; }

	[[nodiscard]] float GetPitch() const { return m_Transform.rotation.x; }
	[[nodiscard]] float GetYaw() const { return m_Transform.rotation.y; }
private:
	void UpdateView();

	DirectX::SimpleMath::Matrix m_ViewMatrix;

	DirectX::SimpleMath::Vector2 m_InitialMousePosition = { 0.0f, 0.0f };

	Transform m_Transform;
};