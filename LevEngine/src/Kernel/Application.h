#pragma once

#include "LayerStack.h"
#define NOMINMAX
#include "Window.h"

#include "../Events/ApplicationEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"
#include "GUI/ImGuiLayer.h"

namespace LevEngine
{
struct ApplicationCommandLineArgs
{
	int Count = 0;
	char** Args = nullptr;

	const char* operator[](const int index) const
	{
		LEV_CORE_ASSERT(index < Count);
		return Args[index];
	}
};

struct ApplicationSpecification
{
	String Name = "LevApp";
	uint32_t WindowWidth = 1600;
	uint32_t WindowHeight = 900;
	ApplicationCommandLineArgs CommandLineArgs;
};
	
class Application
{
public:
	explicit Application(const ApplicationSpecification& specification);
	~Application();

	void Run();
	void Render();
	void Close();

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);
	void OnEvent(Event& e);
	ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; }

	static Application& Get() { return *s_Instance; }
	[[nodiscard]] Window& GetWindow() const { return *m_Window; }
	[[nodiscard]] const ApplicationSpecification& GetSpecification() const { return m_Specification; }
private:
	bool OnWindowClosed(WindowClosedEvent& e);
	bool OnWindowResized(WindowResizedEvent& e);
	bool OnKeyPressed(KeyPressedEvent& e);
	bool OnKeyReleased(KeyReleasedEvent& e);
	bool OnMouseMoved(MouseMovedEvent& e);
	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
	bool OnMouseScrolled(MouseScrolledEvent& e);


	Scope<Window> m_Window;
	bool m_IsRunning = true;

	LayerStack m_LayerStack;

	float m_LastFrameTime = 0.0f;
	bool m_Minimized = false;
	ImGuiLayer* m_ImGuiLayer;
	ApplicationSpecification m_Specification;

	static Application* s_Instance;
};
}
