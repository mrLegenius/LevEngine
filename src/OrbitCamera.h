#pragma once
#include "entt/entt.hpp"
#include "Renderer/Camera/SceneCamera.h"
#include "Scene/System.h"

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
	Transform* m_Target = nullptr;

	float m_Distance = 10;

	float m_AzimuthAngle = 90;
	float m_PolarAngle = 90;

	void RotateAzimuth(float radians);
	void RotatePolar(float radians);
	void Zoom(float value);
	Vector3 CalculatePosition() const;
	Matrix CalculateViewMatrix() const;
};

class OrbitCameraSystem : public System
{
	void Update(const float deltaTime, entt::registry& registry) override
	{
		const auto view = registry.view<Transform, OrbitCamera>();
		for (const auto entity : view)
		{
			auto [transform, camera] = view.get<Transform, OrbitCamera>(entity);

			camera.Update(deltaTime);

			transform.SetWorldPosition(camera.position);
			transform.SetWorldRotation(camera.rotation);
		}
	}
};