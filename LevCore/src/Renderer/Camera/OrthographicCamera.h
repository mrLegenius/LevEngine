#pragma once
#include "glm/glm.hpp"

namespace LevEngine
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float aspectRatio, float orthographicSize);
		~OrthographicCamera() = default;

		[[nodiscard]] const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position)
		{
			m_Position = position;
			RecalculateViewMatrix();
		}

		[[nodiscard]] float GetRotation() const { return m_Rotation; }
		void SetRotation(const float rotation)
		{
			m_Rotation = rotation;
			RecalculateViewMatrix();
		}

		void SetAspectRatio(float aspectRatio);
		void SetOrthographicSize(float size);

		[[nodiscard]] glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
		[[nodiscard]] glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
	
	private:
		void RecalculateProjectionMatrix();
		void RecalculateViewMatrix();
		
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		
		glm::vec3 m_Position {0.0f};
		float m_Rotation = 0.0f;

		float m_AspectRatio;
		float m_OrthographicSize;
	};
}
