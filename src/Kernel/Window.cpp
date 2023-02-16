#include "Window.h"

#include <iostream>
#include "../Events/ApplicationEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	
	switch (umessage)
	{
	case WM_CLOSE:
	{
		auto& data = *reinterpret_cast<Window::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		WindowClosedEvent event;
		data.eventCallback(event);
		return 0;
	}
	case WM_SIZE:
	{
		auto data = reinterpret_cast<Window::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (data == nullptr)
			return DefWindowProc(hwnd, umessage, wparam, lparam);

		data->width = LOWORD(lparam);
		data->height = HIWORD(lparam);

		WindowResizedEvent event(data->width, data->height);
		data->eventCallback(event);
		return 0;
	}
	case WM_CHAR:
	{
		auto& data = *reinterpret_cast<Window::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		const auto charCode = static_cast<KeyCode>(wparam);
		KeyTypedEvent event(charCode);
		data.eventCallback(event);
		return 0;
	}
	case WM_KEYDOWN:
	{
		auto& data = *reinterpret_cast<Window::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		const auto keyCode = static_cast<KeyCode>(wparam);

		std::cout << "Key pressed: " << static_cast<int>(keyCode) << std::endl;

		KeyPressedEvent event(keyCode, 0);
		data.eventCallback(event);

		if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
		return 0;
	}
	case WM_KEYUP:
	{
		auto& data = *reinterpret_cast<Window::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		const auto keyCode = static_cast<KeyCode>(wparam);

		KeyReleasedEvent event(keyCode);
		data.eventCallback(event);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		auto& data = *reinterpret_cast<Window::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		const int xPos = LOWORD(lparam);
		const int yPos = HIWORD(lparam);
		MouseMovedEvent event((xPos), (yPos));
		data.eventCallback(event);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		auto& data = *reinterpret_cast<Window::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		const int xOffset = LOWORD(lparam);
		const int yOffset = HIWORD(lparam);
		MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
		data.eventCallback(event);
	}
	default:
	{
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}
}

std::unique_ptr<Window> Window::Create(const WindowAttributes& attributes)
{
	return std::make_unique<Window>(attributes);
}

Window::Window(const WindowAttributes& attributes)
{
	Window::Init(attributes);
}

Window::~Window()
{
	Window::Close();
}

void Window::Init(const WindowAttributes& attributes)
{
	m_Data.title = attributes.title;
	m_Data.width = attributes.width;
	m_Data.height = attributes.height;

	std::cout << "Creating window " << attributes.title << " (" << attributes.width << "x" << attributes.height << ")" << std::endl;

	const HINSTANCE hInstance = GetModuleHandle(nullptr);
	WNDCLASSEX wc{};

	const std::wstring widestr = std::wstring(attributes.title.begin(), attributes.title.end());
	const wchar_t* wide_title = widestr.c_str();

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = wide_title;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	RECT windowRect = { 0, 0, static_cast<LONG>(attributes.width), static_cast<LONG>(attributes.height) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	constexpr auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	const auto posX = (GetSystemMetrics(SM_CXSCREEN) - attributes.width) / 2;
	const auto posY = (GetSystemMetrics(SM_CYSCREEN) - attributes.height) / 2;

	m_Window = CreateWindowEx(WS_EX_APPWINDOW, wide_title, wide_title,
		dwStyle,
		posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(m_Window, SW_SHOW);
	SetForegroundWindow(m_Window);
	SetFocus(m_Window);
	SetWindowLongPtr(m_Window, GWLP_USERDATA,
		reinterpret_cast<LONG_PTR>(&m_Data));

	ShowCursor(true);

	m_Context = new D3D11Context();
	m_Context->Init(attributes.width, attributes.height, m_Window);

	SetVSync(true);
}

void Window::Close()
{
	DestroyWindow(m_Window);
	m_Window = nullptr;
}

void Window::Update()
{
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// If windows signals to end the application then exit out.
	if (msg.message == WM_QUIT)
	{
		Close();
		return;
	}

	m_Context->SwapBuffers();
}

void Window::SetWindowTitle(std::string& title)
{
	const std::wstring widestr = std::wstring(title.begin(), title.end());
	const wchar_t* wide_title = widestr.c_str();
	SetWindowText(m_Window, wide_title);
}

void Window::SetVSync(const bool enabled)
{
	m_Data.vSync = enabled;
}

bool Window::IsVSync() const
{
	return m_Data.vSync;
}
