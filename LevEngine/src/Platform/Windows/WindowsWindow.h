#pragma once
#include "Kernel/Window.h"

namespace LevEngine
{
	class WindowsWindow final : public Window
	{
	public:
		explicit WindowsWindow(const WindowAttributes& attributes)
		{
			Init(attributes);
		}
		~WindowsWindow() override { Close(); }
		[[nodiscard]] void* GetNativeWindow() const override { return m_Window; }
		void Init(const WindowAttributes& attributes) override;
		void HandleInput() override;
		void SetWindowTitle(String& title) override;

	protected:
		void ConfineCursor() const override;
		void FreeCursor() const override;
		void ShowCursor() const override;
		void HideCursor() const override;
		void Close() override;
	private:
		HWND m_Window = nullptr;
	};
}
