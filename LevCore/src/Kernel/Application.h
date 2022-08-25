#pragma once
#include <string>

#include "Layer.h"
#include "LayerStack.h"
#include "Window.h"
#include "PointerUtils.h"
#include "../Events/ApplicationEvent.h"

#include "../GUI/ImGuiLayer.h"

namespace LevEngine
{
	class Application
	{
	public:
		
		explicit Application(const std::string& name = "LevEngine App");
		~Application();

		void OnEvent(Event& e);
		
		void Run();
		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static Application& Get() { return *s_Instance; }
		[[nodiscard]] Window& GetWindow() const { return *m_Window; }
	private:
		bool OnWindowClosed(WindowClosedEvent& e);
		bool OnWindowResized(WindowResizedEvent& e);

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_IsRunning = true;
		LayerStack m_LayerStack;
		
		float m_LastFrameTime = 0.0f;
		bool m_Minimized = false;

		static Application* s_Instance;
	};

	Application* CreateApplication();
}
