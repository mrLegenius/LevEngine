#pragma once

#include "LayerStack.h"
#define NOMINMAX
#include "Window.h"

#include "../Events/ApplicationEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"
#include "FMOD/LevFmod.h"
#include "GUI/ImGuiLayer.h"

namespace LevEngine
{
class Application
{
public:
	explicit Application(const String& name = "My3DApp", uint32_t width = 800, uint32_t height = 800);
	~Application();

	void Run();
	void Close();

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);
	void OnEvent(Event& e);
	ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; }

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

	Scope<Window> m_Window;
	bool m_IsRunning = true;

	LayerStack m_LayerStack;

	float m_LastFrameTime = 0.0f;
	bool m_Minimized = false;
	ImGuiLayer* m_ImGuiLayer;

	LevFmod::LevFmod* m_fmod;

	static Application* s_Instance;
};
}
