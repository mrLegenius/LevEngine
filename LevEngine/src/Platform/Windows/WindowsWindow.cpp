#include "levpch.h"
#include "WindowsWindow.h"

#include <windowsx.h>

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Input/KeyCodes.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace LevEngine
{
	LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) noexcept
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
			return true;

		const auto data = reinterpret_cast<Window::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		switch (umessage)
		{
		case WM_CLOSE:
		{
			WindowClosedEvent event;
			data->eventCallback(event);
			return 0;
		}
		case WM_QUIT:
		{
			WindowClosedEvent event;
			data->eventCallback(event);
			return 0;
		}
		case WM_SIZE:
		{
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
			const auto charCode = static_cast<KeyCode>(wparam);
			KeyTypedEvent event(charCode);
			data->eventCallback(event);
			return 0;
		}
		case WM_KEYDOWN:
		{
			const auto keyCode = static_cast<KeyCode>(wparam);

			KeyPressedEvent event(keyCode, 0);
			data->eventCallback(event);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			MouseButtonPressedEvent event(MouseButton::Left);
			data->eventCallback(event);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			MouseButtonPressedEvent event(MouseButton::Right);
			data->eventCallback(event);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			MouseButtonPressedEvent event(MouseButton::Middle);
			data->eventCallback(event);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			MouseButtonReleasedEvent event(MouseButton::Left);
			data->eventCallback(event);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			MouseButtonReleasedEvent event(MouseButton::Right);
			data->eventCallback(event);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			MouseButtonReleasedEvent event(MouseButton::Middle);
			data->eventCallback(event);
			return 0;
		}
		case WM_KEYUP:
		{
			const auto keyCode = static_cast<KeyCode>(wparam);

			KeyReleasedEvent event(keyCode);
			data->eventCallback(event);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			break;
			const int xPos = LOWORD(lparam);
			const int yPos = HIWORD(lparam);

			MouseMovedEvent event((xPos), (yPos));
			data->eventCallback(event);
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			const auto zDelta = GET_WHEEL_DELTA_WPARAM(wparam);
			MouseScrolledEvent event(0.0f, (zDelta));
			data->eventCallback(event);
			return 0;
		}
		case WM_INPUT:
		{
			UINT size;

			// first get the size of the input data
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lparam),
				RID_INPUT,
				nullptr,
				&size,
				sizeof(RAWINPUTHEADER)) == -1)
			{
				break;
			}

			auto lpb = new BYTE[size];

			// read in the input data
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lparam),
				RID_INPUT,
				lpb,
				&size,
				sizeof(RAWINPUTHEADER)) != size)
			{
				delete[] lpb;
				break;
			}
			auto* raw = reinterpret_cast<RAWINPUT*>(lpb);

			if (raw->header.dwType == RIM_TYPEMOUSE &&
				(raw->data.mouse.lLastX != 0 || raw->data.mouse.lLastY != 0))
			{
				MouseMovedEvent event((raw->data.mouse.lLastX), (raw->data.mouse.lLastY));
				data->eventCallback(event);
			}

			delete[] lpb;
			return 0;
		}
		default:
		{
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
		}
	}

	void WindowsWindow::Init(const WindowAttributes& attributes)
	{
		LEV_PROFILE_FUNCTION();

		m_Data.title = attributes.title;
		m_Data.width = attributes.width;
		m_Data.height = attributes.height;

		Log::CoreInfo("Creating window {0} ({1}x{2})", attributes.title, attributes.width, attributes.height);

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

		RAWINPUTDEVICE Rid[2];

		Rid[0].usUsagePage = 0x01;
		Rid[0].usUsage = 0x02;
		Rid[0].dwFlags = 0;   // adds HID mouse and also ignores legacy mouse messages
		Rid[0].hwndTarget = m_Window;

		Rid[1].usUsagePage = 0x01;
		Rid[1].usUsage = 0x06;
		Rid[1].dwFlags = 0;   // adds HID keyboard and also ignores legacy keyboard messages
		Rid[1].hwndTarget = m_Window;

		auto res = RegisterRawInputDevices(Rid, 2, sizeof(Rid[0]));
		if (res == FALSE)
			Log::CoreError("Error while registering raw input devices. Error code {0}", GetLastError());

		m_Context = RendererContext::Create();
		m_Context->Init(attributes.width, attributes.height, m_Window);

		SetVSync(true);
	}

	void WindowsWindow::Close()
	{
		LEV_PROFILE_FUNCTION();

		DestroyWindow(m_Window);
		m_Window = nullptr;
	}

	void WindowsWindow::HandleInput()
	{
		MSG msg = {};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void WindowsWindow::SetWindowTitle(std::string& title)
	{
		LEV_PROFILE_FUNCTION();

		const std::wstring widestr = std::wstring(title.begin(), title.end());
		const wchar_t* wide_title = widestr.c_str();
		SetWindowText(m_Window, wide_title);
	}

	void WindowsWindow::ConfineCursor() const
	{
		LEV_PROFILE_FUNCTION();

		RECT rect;
		GetClientRect(m_Window, &rect);
		MapWindowRect(m_Window, nullptr, reinterpret_cast<POINT*>(&rect), 2);
		ClipCursor(&rect);
	}

	void WindowsWindow::FreeCursor() const
	{
		LEV_PROFILE_FUNCTION();

		ClipCursor(nullptr);
	}

	void WindowsWindow::ShowCursor() const
	{
		LEV_PROFILE_FUNCTION();

		while (::ShowCursor(true) < 0);
	}

	void WindowsWindow::HideCursor() const
	{
		LEV_PROFILE_FUNCTION();

		while (::ShowCursor(false) >= 0);
	}
}
