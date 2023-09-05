#include "pch.h"
#include "EditorCamera.h"

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

		if (Input::IsMouseButtonDown(MouseButton::Right))
		{
			auto rotation = m_Transform.GetLocalRotation().ToEuler() * Math::RadToDeg;
			rotation.y -= delta.x;
			rotation.x -= delta.y;
			m_Transform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(rotation * Math::DegToRad));

			if (Input::IsKeyDown(KeyCode::A))
				m_Transform.MoveLeft(deltaTime * m_MoveSpeed);
			else if (Input::IsKeyDown(KeyCode::D))
				m_Transform.MoveRight(deltaTime * m_MoveSpeed);

			if (Input::IsKeyDown(KeyCode::W))
				m_Transform.MoveForward(deltaTime * m_MoveSpeed);
			else if (Input::IsKeyDown(KeyCode::S))
				m_Transform.MoveBackward(deltaTime * m_MoveSpeed);

			if (Input::IsKeyDown(KeyCode::E))
				m_Transform.MoveUp(deltaTime * m_MoveSpeed);
			else if (Input::IsKeyDown(KeyCode::Q))
				m_Transform.MoveDown(deltaTime * m_MoveSpeed);
		}

		Zoom(deltaTime * zoomSensitivity * Input::GetMouseWheelOffset());
	}

	void EditorCamera::Zoom(const float value)
	{
		m_MoveSpeed = Math::Clamp(m_MoveSpeed + value, 0.5f, 100.0f);
	}
}
