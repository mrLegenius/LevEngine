#pragma once
#include "Renderer/Camera/SceneCamera.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
	class EditorCamera final : public SceneCamera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float nearClip, float farClip, Vector3 initialPosition = Vector3::Zero);

		void OnUpdate(float deltaTime);
		void Zoom(float value);

		[[nodiscard]] const Transform& GetTransform() const { return m_Transform; }
		void UpdateView() { m_Transform.RecalculateModel(); }

	private:

		float m_MoveSpeed = 10.0f;
		Matrix m_ViewMatrix;

		Transform m_Transform;

		Vector2 m_InitialMousePosition = { 0.0f, 0.0f };
	};
}
