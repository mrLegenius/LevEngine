#pragma once

#include "Kernel/Window.h"
#include "Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace LevEngine
{
	class WindowsWindow : public Window
	{
	public:
		explicit WindowsWindow(const WindowAttributes& attributes);
		~WindowsWindow() override;

		void Update() override;

		[[nodiscard]] unsigned int GetWidth() const override { return m_Data.width; }
		[[nodiscard]] unsigned int GetHeight() const override { return m_Data.height; }

		void SetEventCallback(const EventCallbackFn& callback) override
		{
			m_Data.eventCallback = callback;
		}

		void SetVSync(bool enabled) override;
		[[nodiscard]] bool IsVSync() const override;

		[[nodiscard]] void* GetNativeWindow() const override { return m_Window; }
	
	private:
		virtual void Init(const WindowAttributes& attributes);
		virtual void Close();

		GLFWwindow* m_Window = nullptr;
		GraphicsContext* m_Context = nullptr;
		
		struct WindowData
		{
			std::string title;
			unsigned int width = 0;
			unsigned int height = 0;

			bool vSync = false;

			EventCallbackFn eventCallback;
		};

		WindowData m_Data;
	};
}
