#include "pch.h"
#include "Application.h"

#include <chrono>

#include "Time.h"
#include "Utils.h"
#include "../Renderer/Renderer.h"
#include "../Events/ApplicationEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"
#include "../Input/Input.h"
#include "../Events/Event.h"
#include "Math/Random.h"

namespace LevEngine
{
Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name, const uint32_t width, const uint32_t height)
{
	LEV_PROFILE_FUNCTION();

	LEV_CORE_ASSERT(!s_Instance, "Only one application is allowed");
	s_Instance = this;

	m_Window = Window::Create(WindowAttributes(name, width, height));
	m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	Renderer::Init();
	Random::Init();

	m_ImGuiLayer = new ImGuiLayer;
	PushOverlay(m_ImGuiLayer);

	Time::s_StartupTime = std::chrono::high_resolution_clock::now();
}

Application::~Application()
{
	Renderer::Shutdown();
}

void Application::Run()
{
	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	float totalTime = 0;
	unsigned int frameCount = 0;

	while (m_IsRunning)
	{
		auto curTime = std::chrono::steady_clock::now();
		const float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
		Time::SetDeltaTime(deltaTime);
		PrevTime = curTime;

		totalTime += deltaTime;
		frameCount++;

		if (totalTime > 1.0f)
		{
			const float fps = frameCount / totalTime;

			totalTime -= 1.0f;

			std::string text("FPS: ");
			text.append(std::to_string(fps));
			m_Window->SetWindowTitle(text);

			frameCount = 0;
		}

		if (deltaTime > 1.0f) // Maybe breakpoint is hit
			continue;

		m_Window->HandleInput();

		if (!m_Minimized)
		{
			LEV_PROFILE_SCOPE("LayerStack OnUpdate");
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(deltaTime);
		}

		//We need to bind main render target before drawing GUI
		//TODO: Maybe move to another place
		m_Window->GetContext()->GetRenderTarget()->Bind();

		m_ImGuiLayer->Begin();
		{
			LEV_PROFILE_SCOPE("LayerStack OnGUIRender");
			for (Layer* layer : m_LayerStack)
				layer->OnGUIRender();
		}
		m_ImGuiLayer->End();

		Input::Reset();
		m_Window->Update();
	}
}

void Application::Close()
{
	m_IsRunning = false;
}

void Application::PushLayer(Layer* layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay)
{
	LEV_PROFILE_FUNCTION();

	m_LayerStack.PushOverlay(overlay);
	overlay->OnAttach();
}

void Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);

	dispatcher.Dispatch<WindowClosedEvent>(BIND_EVENT_FN(Application::OnWindowClosed));
	dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(Application::OnWindowResized));
	dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(Application::OnKeyPressed));
	dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(Application::OnKeyReleased));
	dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(Application::OnMouseMoved));
	dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(Application::OnMouseButtonPressed));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(Application::OnMouseButtonReleased));
	dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Application::OnMouseScrolled));

	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
	{
		(*--it)->OnEvent(e);
		if (e.handled)
			break;
	}
}

bool Application::OnWindowClosed(WindowClosedEvent& e)
{
	Close();
	return true;
}

bool Application::OnWindowResized(WindowResizedEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		m_Minimized = true;

		return false;
	}

	m_Minimized = false;

	return false;
}

bool Application::OnKeyPressed(KeyPressedEvent& e)
{
	Input::OnKeyPressed(e.GetKeyCode());
	return false;
}

bool Application::OnKeyReleased(KeyReleasedEvent& e)
{
	Input::OnKeyReleased(e.GetKeyCode());
	return false;
}

bool Application::OnMouseMoved(MouseMovedEvent& e)
{
	Input::OnMouseMoved(e.GetX(), e.GetY());
	return false;
}

bool Application::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	Input::OnMouseButtonPressed(e.GetMouseButton());
	return false;
}

bool Application::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	Input::OnMouseButtonReleased(e.GetMouseButton());
	return false;
}

bool Application::OnMouseScrolled(MouseScrolledEvent& e)
{
	Input::OnMouseScrolled(e.GetYOffset());
	return false;
}
}
