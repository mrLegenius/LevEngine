#include "OrthographicCameraController.h"

#include "Input/Input.h"
#include "Kernel/Time.h"
#include "Debug/Instrumentor.h"

namespace LevEngine
{
	OrthographicCameraController::OrthographicCameraController(
		const float aspectRatio, 
		const bool isRotatable) :
			m_IsRotatable(isRotatable),
			m_Camera(aspectRatio, m_Zoom) { }

	void OrthographicCameraController::Update()
	{
		LEV_PROFILE_FUNCTION();
		
		const auto deltaTime = Time::GetDeltaTime();

		auto cameraPosition = m_Camera.GetPosition();
		
		if(Input::IsKeyPressed(KeyCode::A) || Input::IsKeyPressed(KeyCode::Left))
		{
			cameraPosition.x -= m_MoveSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(KeyCode::D) || Input::IsKeyPressed(KeyCode::Right))
		{
			cameraPosition.x += m_MoveSpeed * deltaTime;
		}

		if (Input::IsKeyPressed(KeyCode::W) || Input::IsKeyPressed(KeyCode::Up))
		{
			cameraPosition.y += m_MoveSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(KeyCode::S) || Input::IsKeyPressed(KeyCode::Down))
		{
			cameraPosition.y -= m_MoveSpeed * deltaTime;
		}

		m_Camera.SetPosition(cameraPosition);

		
		if(!m_IsRotatable) return;

		auto cameraRotation = m_Camera.GetRotation();
		
		if (Input::IsKeyPressed(KeyCode::Q))
		{
			cameraRotation += m_RotationSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(KeyCode::E))
		{
			cameraRotation -= m_RotationSpeed * deltaTime;
		}

		if (cameraRotation >= 180.0f)
			cameraRotation -= 360.0f;
		else if (cameraRotation <= -180.0f)
			cameraRotation += 360.0f;
		
		m_Camera.SetRotation(cameraRotation);
	}

	void OrthographicCameraController::OnEvent(Event& event)
	{
		LEV_PROFILE_FUNCTION();
		
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseScrolledEvent>(LEV_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizedEvent>(LEV_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		LEV_PROFILE_FUNCTION();
		
		m_Zoom += e.GetYOffset() * 0.25f;

		m_Zoom = std::max(m_Zoom, 0.1f);
		
		m_Camera.SetOrthographicSize(m_Zoom);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizedEvent& e)
	{
		LEV_PROFILE_FUNCTION();
		
		const auto aspectRatio = static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight());

		m_Camera.SetAspectRatio(aspectRatio);
		return false;
	}
}
