#include "pch.h"
#include "Window.h"

#include <imgui.h>

#include "Platform/Windows/WindowsWindow.h"

namespace LevEngine
{
std::unique_ptr<Window> Window::Create(const WindowAttributes& attributes)
{
	LEV_PROFILE_FUNCTION();

	//TODO: Choose window for each platform
	return CreateScope<WindowsWindow>(attributes);
}

void Window::Update() const
{
	m_Context->SwapBuffers();
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