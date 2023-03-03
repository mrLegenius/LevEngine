#pragma once
#include <memory>

#include "../Renderer/Camera/SceneCamera.h"

class OrbitCamera final : public SceneCamera
{
public:
	OrbitCamera(float fov, float nearClip, float farClip);
	
	void SetTarget(const std::shared_ptr<Transform>& target) { m_Target = target; }

	void Update(float deltaTime);
private:
	std::shared_ptr<Transform> m_Target;
	DirectX::SimpleMath::Vector2 m_InitialMousePosition = { 0.0f, 0.0f };

	float m_Distance = 10;

	float m_AzimuthAngle;
	float m_PolarAngle;

	void RotateAzimuth(float radians);
	void RotatePolar(float radians);
	void Zoom(float value);
	DirectX::SimpleMath::Vector3 CalculatePosition() const;

	void UpdateView() override;
};