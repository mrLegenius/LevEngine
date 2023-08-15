#include "pch.h"
#include "EditorCamera.h"

#include "Input/Input.h"
#include "Input/KeyCodes.h"
#include "Math/Math.h"

namespace LevEngine
{
	EditorCamera::EditorCamera(const float fov, const float nearClip, const float farClip, Vector3 initialPosition)
	{
		m_Transform.SetLocalPosition(initialPosition);
		SetPerspective(fov, nearClip, farClip);
	}

	void EditorCamera::OnUpdate(const float deltaTime)
	{
		constexpr auto rotationSpeed = 45;
		constexpr auto zoomSensitivity = 0.5f;

		const DirectX::SimpleMath::Vector2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		if (m_InitialMousePosition.x == 0 && m_InitialMousePosition.y == 0)
			m_InitialMousePosition = mouse;

		const auto delta = (mouse - m_InitialMousePosition) * rotationSpeed * deltaTime;
		m_InitialMousePosition = mouse;

		if (Input::IsKeyDown(KeyCode::A))
			m_Transform.MoveLeft(deltaTime * m_MoveSpeed);
		else if (Input::IsKeyDown(KeyCode::D))
			m_Transform.MoveRight(deltaTime * m_MoveSpeed);

		if (Input::IsKeyDown(KeyCode::W))
			m_Transform.MoveForward(deltaTime * m_MoveSpeed);
		else if (Input::IsKeyDown(KeyCode::S))
			m_Transform.MoveBackward(deltaTime * m_MoveSpeed);

		auto rotation = m_Transform.GetRotationDegrees();

		if (Input::IsKeyDown(KeyCode::Q))
			rotation.z += deltaTime * rotationSpeed;
		else if (Input::IsKeyDown(KeyCode::E))
			rotation.z -= deltaTime * rotationSpeed;

		rotation.y -= delta.x;
		rotation.x -= delta.y;

		m_Transform.SetWorldRotation(rotation);

		Zoom(deltaTime * zoomSensitivity * Input::GetMouseWheelOffset());
	}

	void EditorCamera::Zoom(const float value)
	{
		m_MoveSpeed = Math::Clamp(m_MoveSpeed + value, 0.5f, 100.0f);
	}
}
