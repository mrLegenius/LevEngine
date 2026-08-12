#pragma once

namespace LevEngine
{
	class EditorCamera final : public SceneCamera
	{
	public:
		EditorCamera(float fov, float nearClip, float farClip, Vector3 initialPosition = Vector3::Zero);

		void OnUpdate(float deltaTime);
		void Zoom(float value);

		//Moves the camera so the entity with all of its children fits into the view, the orientation is kept
		void Focus(Entity entity);
		void Focus(Vector3 center, float radius);
		void FocusOnBounds(Vector3 min, Vector3 max);

		//Builds a world space ray going through a point of the viewport, the point is in pixels from its left top
		[[nodiscard]] Ray GetViewportRay(Vector2 point, Vector2 viewportSize) const;

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
