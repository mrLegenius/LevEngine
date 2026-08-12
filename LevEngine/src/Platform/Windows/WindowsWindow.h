#pragma once

#include "Kernel/Core.h"
#include "Kernel/Window.h"

namespace LevEngine
{
	class LEV_API WindowsWindow final : public Window
	{
	public:
		explicit WindowsWindow(const Ref<RenderDevice>& renderDevice, const WindowAttributes& attributes)
		{
			Init(renderDevice, attributes);
		}
		~WindowsWindow() override { Close(); }
		[[nodiscard]] void* GetNativeWindow() const override { return m_Window; }
		void Init(const Ref<RenderDevice>& renderDevice, const WindowAttributes& attributes) override;
		void HandleInput() override;
		void SetWindowTitle(String& title) override;
		void SetCursorPosition(uint32_t x, uint32_t y) override;
		void Show() override;
		void Minimize() override;
		void Maximize() override;
		void Restore() override;
		[[nodiscard]] bool IsMaximized() const override;
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
