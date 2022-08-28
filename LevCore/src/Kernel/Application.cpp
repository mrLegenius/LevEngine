#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Events/Event.h"

#include "Renderer/Renderer.h"

#include "Time.h"

namespace LevEngine
{
	Application* Application::s_Instance = nullptr;
	
	Application::Application(const std::string& name)
	{
		LEV_PROFILE_FUNCTION();
		
		LEV_CORE_ASSERT(!s_Instance, "Only one applications allowed");
		s_Instance = this;
		
		m_Window = Window::Create(WindowAttributes(name));
		m_Window->SetEventCallback(LEV_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();
		
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
		LEV_PROFILE_FUNCTION();
		
		while (m_IsRunning)
		{
			LEV_PROFILE_SCOPE("Run loop");
			
			const auto time = Time::GetTimeSinceStartup();
            Time::s_DeltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if(!m_Minimized)
			{
				LEV_PROFILE_SCOPE("LayerStack OnUpdate");
				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate();
				}
			}

			m_ImGuiLayer->Begin();
			{
				LEV_PROFILE_SCOPE("LayerStack OnImGuiRender");
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
			
			m_Window->Update();
		}
	}

	void Application::Close()
	{
		m_IsRunning = false;
	}

	void Application::OnEvent(Event& e)
	{
		LEV_PROFILE_FUNCTION();
		
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowClosedEvent>(LEV_BIND_EVENT_FN(Application::OnWindowClosed));
		dispatcher.Dispatch<WindowResizedEvent>(LEV_BIND_EVENT_FN(Application::OnWindowResized));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		LEV_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		LEV_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}
	
	bool Application::OnWindowClosed(WindowClosedEvent& e)
	{
		Close();
		return true;
	}

	bool Application::OnWindowResized(WindowResizedEvent& e)
	{
		LEV_PROFILE_FUNCTION();
		
		if(e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			
			return false;
		}

		m_Minimized = false;

		Renderer::OnWindowResized(e.GetWidth(), e.GetHeight());
		
		return false;
	}
}