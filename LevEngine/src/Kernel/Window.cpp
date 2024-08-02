#include "levpch.h"
#include "Window.h"

#include <imgui.h>

#include "Platform/Windows/WindowsWindow.h"
#include "Renderer/RenderContext.h"

namespace LevEngine
{
Scope<Window> Window::Create(const Ref<RenderDevice>& renderDevice, const WindowAttributes& attributes)
{
	LEV_PROFILE_FUNCTION();

	//TODO: Choose window for each platform
	return CreateScope<WindowsWindow>(renderDevice, attributes);
}

void Window::Update() const
{
	m_Context->SwapBuffers(IsVSync());
}

void Window::EnableCursor()
{
	LEV_PROFILE_FUNCTION();

	m_CursorEnabled = true;
	ShowCursor();
	FreeCursor();
}

void Window::DisableCursor()
{
	LEV_PROFILE_FUNCTION();

	m_CursorEnabled = false;
	HideCursor();
	ConfineCursor();
}
}
