#include "WindowsWindow.h"

#include "Kernel/Window.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "Platforms/OpenGL/OpenGLContext.h"
#include "glfw/glfw3.h"

namespace LevEngine
{
	static bool is_window_initialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		Log::Error("GLFW Error ({0}): {1}", error, description);
	}

	Scope<Window> Window::Create(const WindowAttributes& attributes)
	{
		return CreateScope<WindowsWindow>(attributes);
	}

	WindowsWindow::WindowsWindow(const WindowAttributes& attributes)
	{
		LEV_PROFILE_FUNCTION();
		
		WindowsWindow::Init(attributes);
	}

	WindowsWindow::~WindowsWindow()
	{
		LEV_PROFILE_FUNCTION();
		
		WindowsWindow::Close();
	}

	void WindowsWindow::Init(const WindowAttributes& attributes)
	{
		LEV_PROFILE_FUNCTION();

		m_Data.title = attributes.title;
		m_Data.width = attributes.width;
		m_Data.height = attributes.height;

		Log::CoreInfo("Creating window {0} ({1}x{2})",
			attributes.title, attributes.width, attributes.height);

		if (!is_window_initialized)
		{
			LEV_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			LEV_CORE_ASSERT(success, "Could not initialize GLFW");

			glfwSetErrorCallback(GLFWErrorCallback);
			is_window_initialized = true;
		}

		{
			LEV_PROFILE_SCOPE("glfwCreateWindow");

			m_Window = glfwCreateWindow(
				static_cast<int>(m_Data.width),
				static_cast<int>(m_Data.height),
				m_Data.title.c_str(),
				nullptr,
				nullptr);
		}
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();
		
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		//Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			data.width = width;
			data.height = height;

			WindowResizedEvent event(width, height);
			data.eventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			WindowClosedEvent event;
			data.eventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			const auto keyCode = static_cast<KeyCode>(key);
			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(keyCode, 0);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(keyCode);
				data.eventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				//TODO: find a repeat count
				KeyPressedEvent event(keyCode, 1);
				data.eventCallback(event);
				break;
			}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int character)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			const auto charCode = static_cast<KeyCode>(character);
			KeyTypedEvent event(charCode);
			data.eventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			const auto buttonCode = static_cast<MouseButton>(button);
			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(buttonCode);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(buttonCode);
				data.eventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
			data.eventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
			data.eventCallback(event);
		});
	}

	void WindowsWindow::Close()
	{
		LEV_PROFILE_FUNCTION();
		
		glfwDestroyWindow(m_Window);
	}
	
	void WindowsWindow::Update()
	{
		LEV_PROFILE_FUNCTION();
		
		glfwPollEvents();
		m_Context->SwapBuffers();
	}
	
	void WindowsWindow::SetVSync(const bool enabled)
	{
		LEV_PROFILE_FUNCTION();
		
		glfwSwapInterval(enabled ? 1 : 0);

		m_Data.vSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.vSync;
	}
}
