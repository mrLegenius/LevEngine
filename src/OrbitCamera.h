#pragma once
#include <memory>

#include "Renderer/Camera/SceneCamera.h"

class OrbitCamera final : public SceneCamera
{
public:
	OrbitCamera(float fov, float nearClip, float farClip);
	OrbitCamera(float fov, float nearClip, float farClip, const std::shared_ptr<Transform>& target);
	
	void SetTarget(const std::shared_ptr<Transform>& target) { m_Target = target; }

	void Update(float deltaTime) override;

private:
	std::shared_ptr<Transform> m_Target;

	float m_Distance = 10;

	float m_AzimuthAngle = 0;
	float m_PolarAngle = DirectX::XM_PIDIV2;

	void RotateAzimuth(float radians);
	void RotatePolar(float radians);
	void Zoom(float value);
	Vector3 CalculatePosition() const;

	void UpdateView() override;
};