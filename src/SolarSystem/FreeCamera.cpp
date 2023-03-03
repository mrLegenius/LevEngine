#include "FreeCamera.h"

#include <iostream>

#include "../Input/Input.h"
#include "../Input/KeyCodes.h"
#include "../Components/Transform.h"

FreeCamera::FreeCamera(const float fov, const float nearClip, const float farClip)
{
	SetPerspective(fov, nearClip, farClip);
	UpdateView();
}

void FreeCamera::Update(float deltaTime)
{
	constexpr auto moveSpeed = 100;
	constexpr auto rotationSpeed = 1;

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

	if (Input::IsKeyDown(KeyCode::Q))
		m_Transform.rotation.z += (deltaTime * rotationSpeed);
	else if (Input::IsKeyDown(KeyCode::E))
		m_Transform.rotation.z -= (deltaTime * rotationSpeed);

	m_Transform.rotation.y -= delta.x;
	m_Transform.rotation.x -= delta.y;

	UpdateView();
}
