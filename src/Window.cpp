#include "Window.h"

#include <iostream>

static bool is_window_initialized = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;

		if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
		return 0;
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
