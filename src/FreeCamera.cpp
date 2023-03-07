#include "FreeCamera.h"

#include <iostream>

#include "Input/Input.h"
#include "Input/KeyCodes.h"
#include "Components/Transform.h"

FreeCamera::FreeCamera(const float fov, const float nearClip, const float farClip)
{
	SetPerspective(fov, nearClip, farClip);
	SceneCamera::UpdateView();
}

void FreeCamera::Zoom(const float value)
{
	auto orthoSize = GetOrthographicSize();
	orthoSize += value;

	if (orthoSize < 0.1f)
		orthoSize = 0.1f;

	SetOrthographicSize(orthoSize);
}

void FreeCamera::Update(float deltaTime)
{
	constexpr auto moveSpeed = 100;
	constexpr auto rotationSpeed = 10;

	const DirectX::SimpleMath::Vector2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
	if (m_InitialMousePosition.x == 0 && m_InitialMousePosition.y == 0)
		m_InitialMousePosition = mouse;

	const auto delta = (mouse - m_InitialMousePosition) * rotationSpeed * deltaTime;
	m_InitialMousePosition = mouse;

	if (Input::IsKeyDown(KeyCode::A))
		m_Transform.MoveLeft(deltaTime * moveSpeed);
	else if (Input::IsKeyDown(KeyCode::D))
		m_Transform.MoveRight(deltaTime * moveSpeed);

	if (Input::IsKeyDown(KeyCode::W))
		m_Transform.MoveForward(deltaTime * moveSpeed);
	else if (Input::IsKeyDown(KeyCode::S))
		m_Transform.MoveBackward(deltaTime * moveSpeed);

	auto rotation = m_Transform.GetRotationDegrees();

	if (Input::IsKeyDown(KeyCode::Q))
		rotation.z += (deltaTime * rotationSpeed);
	else if (Input::IsKeyDown(KeyCode::E))
		rotation.z -= (deltaTime * rotationSpeed);

	rotation.y -= delta.x;
	rotation.x -= delta.y;

	m_Transform.SetRotation(rotation);

	constexpr auto ZoomSensitivity = 0.5f;
	Zoom(deltaTime * ZoomSensitivity * -Input::GetMouseWheelOffset());

	UpdateView();
}
