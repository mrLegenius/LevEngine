#pragma once

#include "Kernel/Core.h"

#include <Renderer/3D/MeshLoading/AnimationLoader.h>

#include "../Events/Event.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
	class RenderContext;

	struct LEV_API WindowAttributes
{
	String title;
	uint32_t width;
	uint32_t height;
	//When set, the system caption is removed and the client is expected to draw its own title bar
	bool customTitleBar;

	explicit WindowAttributes(String title = "LevEngine",
		const unsigned int width = 1280,
		const unsigned int height = 720,
		const bool customTitleBar = false)
		: title(Move(title)),
		width(width),
		height(height),
		customTitleBar(customTitleBar) { }
};

class LEV_API Window
{
public:
	using EventCallbackFn = Action<Event&>;

	virtual ~Window() = default;

	virtual void HandleInput() = 0;
	void Update() const;

	[[nodiscard]] unsigned int GetWidth() const { return m_Data.width; }
	[[nodiscard]] unsigned int GetHeight() const { return m_Data.height; }

	void SetEventCallback(const EventCallbackFn& callback)
	{
		m_Data.eventCallback = callback;
	}

	virtual void SetWindowTitle(String& title) = 0;
	void SetVSync(const bool enabled) { m_Data.vSync = enabled; }
	[[nodiscard]] bool IsVSync() const { return m_Data.vSync; }

	virtual void Minimize() = 0;
	virtual void Maximize() = 0;
	virtual void Restore() = 0;
	[[nodiscard]] virtual bool IsMaximized() const = 0;

	[[nodiscard]] bool HasCustomTitleBar() const { return m_Data.customTitleBar; }
	//A custom title bar is drawn by the client, so the window has to be told every frame
	//which part of the client area behaves as the caption (dragging, snapping, system menu)
	void SetTitleBarHovered(const bool hovered) { m_Data.titleBarHovered = hovered; }

	void EnableCursor();
	void DisableCursor();
	virtual void SetCursorPosition(uint32_t x, uint32_t y) = 0;

	[[nodiscard]] const Ref<RenderContext>& GetContext() const { return m_Context; }

	[[nodiscard]] virtual void* GetNativeWindow() const = 0;

	static Scope<Window> Create(const Ref<RenderDevice>& renderDevice, const WindowAttributes& attributes = WindowAttributes());

protected:
	virtual void ConfineCursor() const = 0;
	virtual void FreeCursor() const = 0;

	virtual void ShowCursor() const = 0;
	virtual void HideCursor() const = 0;

	virtual void Init(const Ref<RenderDevice>& renderDevice, const WindowAttributes& attributes) = 0;
	virtual void Close() = 0;

	Ref<RenderContext> m_Context = nullptr;
	bool m_CursorEnabled = true;

public:
	struct WindowData
	{
		String title;
		unsigned int width = 0;
		unsigned int height = 0;

		bool vSync = false;

		bool customTitleBar = false;
		bool titleBarHovered = false;

		EventCallbackFn eventCallback;
	};

	WindowData m_Data;
};
}

