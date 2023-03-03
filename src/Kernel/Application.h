#pragma once
#include <memory>
#include <string>

#include "LayerStack.h"
#include "Window.h"

#include "../Events/ApplicationEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"

class Application
{
public:
	explicit Application(const std::string& name = "My3DApp", uint32_t width = 800, uint32_t height = 800);
	~Application();

	void Run();
	void Close();

	void PushLayer(Layer* layer);
	void OnEvent(Event& e);

	static Application& Get() { return *s_Instance; }
	[[nodiscard]] Window& GetWindow() const { return *m_Window; }
private:
	bool OnWindowClosed(WindowClosedEvent& e);
	bool OnWindowResized(WindowResizedEvent& e);
	bool OnKeyPressed(KeyPressedEvent& e);
	bool OnKeyReleased(KeyReleasedEvent& e);
	bool OnMouseMoved(MouseMovedEvent& e);
	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
	bool OnMouseScrolled(MouseScrolledEvent& e);

	std::unique_ptr<Window> m_Window;
	bool m_IsRunning = true;

	LayerStack m_LayerStack;

	float m_LastFrameTime = 0.0f;
	bool m_Minimized = false;

	static Application* s_Instance;
};

