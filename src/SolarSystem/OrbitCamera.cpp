#include "OrbitCamera.h"

#include <iostream>

#include "../Input/Input.h"

OrbitCamera::OrbitCamera(float fov, float nearClip, float farClip)
{
	SetPerspective(fov, nearClip, farClip);
	UpdateView();
}

OrbitCamera::OrbitCamera(float fov, float nearClip, float farClip, const std::shared_ptr<Transform>& target) : OrbitCamera(fov, nearClip, farClip)
{
	SetTarget(target);
}

void OrbitCamera::RotateAzimuth(const float radians)
{
	m_AzimuthAngle += radians;

	// Keep azimuth angle within range <0..2PI) - it's not necessary, just to have it nicely output
	constexpr auto fullCircle = DirectX::XM_2PI;
	m_AzimuthAngle = fmodf(m_AzimuthAngle, fullCircle);
	if (m_AzimuthAngle < 0.0f) {
		m_AzimuthAngle = fullCircle + m_AzimuthAngle;
	}
}

void OrbitCamera::RotatePolar(const float radians)
{
	m_PolarAngle += radians;

	constexpr auto polarCap = DirectX::XM_PIDIV2 - 0.001f;
	if (m_PolarAngle > polarCap) {
		m_PolarAngle = polarCap;
	}

	if (m_PolarAngle < -polarCap) {
		m_PolarAngle = -polarCap;
	}
}

void OrbitCamera::Zoom(const float value)
{
	m_Distance += value;
	if (m_Distance < 1) {
		m_Distance = 1;
	}

	auto orthoSize = GetOrthographicSize();
	orthoSize += value;

	if (orthoSize < 0.1f)
		orthoSize = 0.1f;

	SetOrthographicSize(orthoSize);
}

DirectX::SimpleMath::Vector3 OrbitCamera::CalculatePosition() const
{
	// Calculate sines / cosines of angles
	const auto sineAzimuth = sin(m_AzimuthAngle);
	const auto cosineAzimuth = cos(m_AzimuthAngle);
	const auto sinePolar = sin(m_PolarAngle);
	const auto cosinePolar = cos(m_PolarAngle);

	// Calculate eye position out of them
	auto x = m_Distance * cosinePolar * cosineAzimuth;
	auto y = m_Distance * sinePolar;
	auto z = m_Distance * cosinePolar * sineAzimuth;

	if (m_Target)
	{
		x += m_Target->GetPosition().x;
		y += m_Target->GetPosition().y;
		z += m_Target->GetPosition().z;
	}

	return { x, y, z };
}

void OrbitCamera::UpdateView()
{
	const auto targetPosition = m_Target ? m_Target->GetPosition() : DirectX::SimpleMath::Vector3::Zero;
		
	if (m_Transform.GetPosition() == targetPosition) return;

	m_ViewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_Transform.GetPosition(), targetPosition, DirectX::SimpleMath::Vector3::Up);
}

void OrbitCamera::Update(const float deltaTime)
{
	constexpr auto ZoomSensitivity = 0.5f;
	constexpr auto rotationSpeed = 1;

	const auto [mouseDeltaX, mouseDeltaY] = Input::GetMouseDelta();

	const auto deltaX = mouseDeltaX * rotationSpeed * deltaTime;
	const auto deltaY = mouseDeltaY * rotationSpeed * deltaTime;

	Zoom(deltaTime * ZoomSensitivity * -Input::GetMouseWheelOffset());

	RotatePolar(deltaY);
	RotateAzimuth(deltaX);

	m_Transform.SetPosition(CalculatePosition());

	UpdateView();
}

