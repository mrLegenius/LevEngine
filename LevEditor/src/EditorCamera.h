﻿#pragma once

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

		void ResetInitialMousePosition()
		{
			m_InitialMousePosition = Vector2{ 0.0f, 0.0f };
		}

	private:

		float m_MoveSpeed = 10.0f;

		Transform m_Transform;

		Vector2 m_InitialMousePosition = { 0.0f, 0.0f };
	};
}
