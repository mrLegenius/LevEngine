#include "levpch.h"
#include "WindowsWindow.h"

#include <windowsx.h>

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Input/KeyCodes.h"
#include "Renderer/RenderContext.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace LevEngine
{
	WPARAM MapLeftRightKeys(const WPARAM virtualKey, const LPARAM lParam)
	{
		const UINT scancode = (lParam & 0x00ff0000) >> 16;
		const int extended  = (lParam & 0x01000000) != 0;

		switch (virtualKey) {
		case VK_SHIFT:
			return MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
		case VK_CONTROL:
			return extended ? VK_RCONTROL : VK_LCONTROL;
		case VK_MENU:
			return extended ? VK_RMENU : VK_LMENU;
		default:
			// not a key we map from generic to left/right specialized
			//  just return it.
			return virtualKey;
		}
	}

	static bool IsWindowMaximized(const HWND hwnd)
	{
		WINDOWPLACEMENT placement{};
		placement.length = sizeof(WINDOWPLACEMENT);

		if (!GetWindowPlacement(hwnd, &placement))
			return false;

		return placement.showCmd == SW_SHOWMAXIMIZED;
	}

	static int GetResizeBorderThickness(const HWND hwnd)
	{
		const UINT dpi = GetDpiForWindow(hwnd);
		return GetSystemMetricsForDpi(SM_CYFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) noexcept
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
			return true;

		const auto data = reinterpret_cast<Window::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		switch (umessage)
		{
		case WM_NCCREATE:
		{
			//The window data has to be reachable from the very first frame messages
			//(WM_NCCALCSIZE arrives while we are still inside CreateWindowEx)
			const auto createStruct = reinterpret_cast<CREATESTRUCT*>(lparam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
			break;
		}
		case WM_NCCALCSIZE:
		{
			if (data == nullptr || !data->customTitleBar || !wparam)
				break;

			const UINT dpi = GetDpiForWindow(hwnd);
			const int frameX = GetSystemMetricsForDpi(SM_CXFRAME, dpi);
			const int frameY = GetSystemMetricsForDpi(SM_CYFRAME, dpi);
			const int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

			//Keeping the side and bottom frames leaves us with free resize hit testing there,
			//while not inseting the top is what removes the system caption
			const auto params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lparam);
			RECT& clientRect = params->rgrc[0];
			clientRect.left += frameX + padding;
			clientRect.right -= frameX + padding;
			clientRect.bottom -= frameY + padding;

			//A maximized window is grown by the invisible frame, so the top has to be pushed back in
			if (IsWindowMaximized(hwnd))
				clientRect.top += padding;

			return 0;
		}
		case WM_NCHITTEST:
		{
			if (data == nullptr || !data->customTitleBar)
				break;

			const LRESULT frameHit = DefWindowProc(hwnd, umessage, wparam, lparam);
			switch (frameHit)
			{
			case HTNOWHERE:
			case HTLEFT:
			case HTRIGHT:
			case HTTOPLEFT:
			case HTTOP:
			case HTTOPRIGHT:
			case HTBOTTOMLEFT:
			case HTBOTTOM:
			case HTBOTTOMRIGHT:
				return frameHit;
			default:
				break;
			}

			//The top resize border lives inside the client area now, so reporting it is on us
			POINT cursor{ GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
			ScreenToClient(hwnd, &cursor);

			if (!IsWindowMaximized(hwnd) && cursor.y >= 0 && cursor.y < GetResizeBorderThickness(hwnd))
				return HTTOP;

			//Dragging, snapping, double click to maximize and the system menu come for free with HTCAPTION
			return data->titleBarHovered ? HTCAPTION : HTCLIENT;
		}
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
		case WM_SETFOCUS:
		{
			if (data == nullptr)
				return DefWindowProc(hwnd, umessage, wparam, lparam);

			WindowFocusEvent event;
			data->eventCallback(event);
			return 0;
		}
		case WM_KILLFOCUS:
		{
			if (data == nullptr)
				return DefWindowProc(hwnd, umessage, wparam, lparam);

			WindowLostFocusEvent event;
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
			const auto keyCode = static_cast<KeyCode>(MapLeftRightKeys(wparam, lparam));
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
			const auto keyCode = static_cast<KeyCode>(MapLeftRightKeys(wparam, lparam));

			KeyReleasedEvent event(keyCode);
			data->eventCallback(event);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			break;
			const int xPos = LOWORD(lparam);
			const int yPos = HIWORD(lparam);

			//MouseMovedEvent event((xPos), (yPos));
			//data->eventCallback(event);
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
				MouseMovedEvent event(static_cast<float>(raw->data.mouse.lLastX), static_cast<float>(raw->data.mouse.lLastY));
				data->eventCallback(event);
			}

			delete[] lpb;
			return 0;
		}
		}

		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}

	void WindowsWindow::Init(const Ref<RenderDevice>& renderDevice, const WindowAttributes& attributes)
	{
		LEV_PROFILE_FUNCTION();

		m_Data.title = attributes.title;
		m_Data.width = attributes.width;
		m_Data.height = attributes.height;
		m_Data.customTitleBar = attributes.customTitleBar;
		//Window messages are dispatched while we are still creating the window,
		//so the callback has to be valid before that
		m_Data.eventCallback = [](Event&) {};

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

		//With a custom title bar the client area covers the whole window, so the requested
		//size is already the window size
		if (!attributes.customTitleBar)
			AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		//The system styles are kept even for a custom title bar: they are what gives us
		//shadows, snapping, minimize animations and resize hit testing
		constexpr auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX;

		const auto posX = (GetSystemMetrics(SM_CXSCREEN) - attributes.width) / 2;
		const auto posY = (GetSystemMetrics(SM_CYSCREEN) - attributes.height) / 2;

		m_Window = CreateWindowEx(WS_EX_APPWINDOW, wide_title, wide_title,
			dwStyle,
			posX, posY,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr, nullptr, hInstance, &m_Data);

		if (attributes.customTitleBar)
		{
			//Force the frame to be recalculated with our WM_NCCALCSIZE
			SetWindowPos(m_Window, nullptr, 0, 0, 0, 0,
				SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
		}

		//The window stays hidden until the application is done loading, see Show()

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

		//The client area is smaller than the requested window size, and with a custom
		//title bar it is not even derived from it, so the actual size is what matters here
		m_Context = RenderContext::Create();
		m_Context->Init(renderDevice, m_Data.width, m_Data.height, IsVSync(), m_Window);

		SetVSync(true);
	}

	void WindowsWindow::Close()
	{
		LEV_PROFILE_FUNCTION();

		SetEventCallback([](auto&){});
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

	void WindowsWindow::SetWindowTitle(String& title)
	{
		LEV_PROFILE_FUNCTION();

		const std::wstring widestr = std::wstring(title.begin(), title.end());
		const wchar_t* wide_title = widestr.c_str();
		SetWindowText(m_Window, wide_title);
	}

	void WindowsWindow::SetCursorPosition(const uint32_t x, const uint32_t y)
	{
		SetCursorPos(x, y);
	}

	void WindowsWindow::Show()
	{
		LEV_PROFILE_FUNCTION();

		ShowWindow(m_Window, SW_SHOW);
		SetForegroundWindow(m_Window);
		SetFocus(m_Window);
	}

	void WindowsWindow::Minimize()
	{
		ShowWindow(m_Window, SW_MINIMIZE);
	}

	void WindowsWindow::Maximize()
	{
		ShowWindow(m_Window, SW_MAXIMIZE);
	}

	void WindowsWindow::Restore()
	{
		ShowWindow(m_Window, SW_RESTORE);
	}

	bool WindowsWindow::IsMaximized() const
	{
		return IsWindowMaximized(m_Window);
	}

	void WindowsWindow::ConfineCursor() const
	{
		LEV_PROFILE_FUNCTION();

		RECT rect;
		GetClientRect(m_Window, &rect);
		MapWindowRect(m_Window, nullptr, &rect);
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
