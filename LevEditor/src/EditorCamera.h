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

		// Placing the camera from code rather than from the mouse. The viewport only ever moves it
		// while the right button is held, which nothing but a hand can do, so a scripted move -- a
		// screenshot from a fixed spot, a flight along a path -- has to go through here.
		void SetPosition(const Vector3 position)
		{
			m_Transform.SetWorldPosition(position);
			UpdateView();
		}

		//<--- Degrees, in the same yaw/pitch/roll order the inspector shows ---<<
		void SetRotationEuler(const Vector3 eulerDegrees)
		{
			m_Transform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(eulerDegrees * Math::DegToRad));
			UpdateView();
		}

		[[nodiscard]] Vector3 GetRotationEuler() const
		{
			return m_Transform.GetWorldRotation().ToEuler() * Math::RadToDeg;
		}

		//<--- Keeps the horizon level, which is what a camera flown by hand does ---<<
		void LookAt(Vector3 target);

		[[nodiscard]] float GetMoveSpeed() const { return m_MoveSpeed; }
		void SetMoveSpeed(const float value) { m_MoveSpeed = Math::Clamp(value, 0.5f, 100.0f); }

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
