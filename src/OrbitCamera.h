#pragma once
#include <memory>

#include "Kernel/PointerUtils.h"
#include "Renderer/Camera/SceneCamera.h"

class OrbitCamera final
{
public:
	OrbitCamera() = default;
	OrbitCamera(const OrbitCamera&) = default;
	OrbitCamera(Transform& target);
	
	void SetTarget(Transform& target) { m_Target = &target; }

	void Update(float deltaTime);

	Vector3 position;
	Quaternion rotation;

private:
	Transform* m_Target;

	float m_Distance = 10;

	float m_AzimuthAngle = 90;
	float m_PolarAngle = 90;

	void RotateAzimuth(float radians);
	void RotatePolar(float radians);
	void Zoom(float value);
	Vector3 CalculatePosition() const;
	Matrix CalculateViewMatrix() const;
};