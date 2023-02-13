#include "Application.h"

#include <chrono>

#include "../Renderer/Renderer.h"

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name, uint32_t width, uint32_t height)
{
	s_Instance = this;

	m_Window = Window::Create(WindowAttributes(name, width, height));

	Renderer::Init();
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

	while (m_Window->GetNativeWindow() != nullptr)
	{
		auto curTime = std::chrono::steady_clock::now();
		const float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
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

		for (Layer* layer : m_LayerStack)
		{
			layer->OnUpdate();
		}

		Renderer::Render();
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