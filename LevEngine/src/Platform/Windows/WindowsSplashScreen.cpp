#include "levpch.h"
#include "Kernel/SplashScreen.h"

#include <dwmapi.h>

#include "stb/include/stb_image.h"

//AlphaBlend and the rounded corners attribute are the only reasons these are needed,
//so they are pulled in here instead of in the project settings
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "dwmapi.lib")

namespace LevEngine
{
	namespace
	{
		constexpr auto k_WindowClassName = L"LevEngineSplashScreen";

		//The layout is authored for 96 dpi and scaled up with the system dpi
		constexpr int k_Width = 520;
		constexpr int k_Height = 300;
		constexpr int k_LogoSize = 128;
		constexpr int k_LogoTop = 32;
		constexpr int k_TitleTop = 180;
		constexpr int k_StatusTop = 232;
		constexpr int k_HorizontalPadding = 24;

		constexpr COLORREF k_BackgroundColor = RGB(24, 24, 27);
		constexpr COLORREF k_BorderColor = RGB(52, 52, 58);
		constexpr COLORREF k_TitleColor = RGB(240, 240, 245);
		constexpr COLORREF k_StatusColor = RGB(150, 150, 158);

		//DWMWA_WINDOW_CORNER_PREFERENCE and DWMWCP_ROUND are missing from older Windows SDKs
		constexpr DWORD k_WindowCornerPreference = 33;
		constexpr DWORD k_RoundedCorners = 2;

		struct SplashScreenData
		{
			HWND window = nullptr;

			HBITMAP logo = nullptr;
			int logoWidth = 0;
			int logoHeight = 0;

			HFONT titleFont = nullptr;
			HFONT statusFont = nullptr;

			UINT dpi = 96;

			String title;
			String status;
		};

		SplashScreenData s_Data;

		int Scaled(const int value) { return MulDiv(value, static_cast<int>(s_Data.dpi), 96); }

		std::wstring ToWide(const String& text) { return std::wstring(text.begin(), text.end()); }

		HFONT CreateSplashFont(const int pointSize, const int weight)
		{
			return CreateFontW(-MulDiv(pointSize, static_cast<int>(s_Data.dpi), 72), 0, 0, 0, weight,
				FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
				CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
		}

		void LoadLogo(const Path& logoPath)
		{
			if (logoPath.empty() || !exists(logoPath))
			{
				Log::CoreWarning("Splash screen logo is not found in {0}", logoPath.string());
				return;
			}

			int width, height, channels;
			stbi_uc* pixels = stbi_load(ToString(logoPath).c_str(), &width, &height, &channels, 4);

			if (pixels == nullptr)
			{
				Log::CoreWarning("Failed to load the splash screen logo from {0}", logoPath.string());
				return;
			}

			BITMAPINFO info{};
			info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			info.bmiHeader.biWidth = width;
			//A negative height is what makes the rows top down, matching the decoded image
			info.bmiHeader.biHeight = -height;
			info.bmiHeader.biPlanes = 1;
			info.bmiHeader.biBitCount = 32;
			info.bmiHeader.biCompression = BI_RGB;

			void* bits = nullptr;
			const HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);

			if (bitmap == nullptr)
			{
				Log::CoreWarning("Failed to create the splash screen logo bitmap. Error code {0}", GetLastError());
				stbi_image_free(pixels);
				return;
			}

			//AlphaBlend expects premultiplied BGRA
			auto* destination = static_cast<uint8_t*>(bits);
			for (int i = 0; i < width * height; ++i)
			{
				const uint32_t alpha = pixels[i * 4 + 3];
				destination[i * 4 + 0] = static_cast<uint8_t>(pixels[i * 4 + 2] * alpha / 255);
				destination[i * 4 + 1] = static_cast<uint8_t>(pixels[i * 4 + 1] * alpha / 255);
				destination[i * 4 + 2] = static_cast<uint8_t>(pixels[i * 4 + 0] * alpha / 255);
				destination[i * 4 + 3] = static_cast<uint8_t>(alpha);
			}

			stbi_image_free(pixels);

			s_Data.logo = bitmap;
			s_Data.logoWidth = width;
			s_Data.logoHeight = height;
		}

		void DrawLogo(const HDC dc, const RECT& client)
		{
			if (s_Data.logo == nullptr) return;

			const int box = Scaled(k_LogoSize);
			const auto aspect = static_cast<float>(s_Data.logoWidth) / static_cast<float>(s_Data.logoHeight);

			int width = box;
			int height = box;
			if (aspect > 1.0f)
				height = static_cast<int>(static_cast<float>(box) / aspect);
			else
				width = static_cast<int>(static_cast<float>(box) * aspect);

			const int x = (client.right - width) / 2;
			const int y = Scaled(k_LogoTop) + (box - height) / 2;

			const HDC logoDc = CreateCompatibleDC(dc);
			const auto previousBitmap = SelectObject(logoDc, s_Data.logo);

			constexpr BLENDFUNCTION blend{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			AlphaBlend(dc, x, y, width, height, logoDc, 0, 0, s_Data.logoWidth, s_Data.logoHeight, blend);

			SelectObject(logoDc, previousBitmap);
			DeleteDC(logoDc);
		}

		void DrawTextLine(const HDC dc, const String& text, const HFONT font,
			const COLORREF color, const int top, const RECT& client)
		{
			if (text.empty() || font == nullptr) return;

			const auto previousFont = SelectObject(dc, font);
			SetTextColor(dc, color);
			SetBkMode(dc, TRANSPARENT);

			RECT rect{ Scaled(k_HorizontalPadding), top, client.right - Scaled(k_HorizontalPadding), client.bottom };
			const auto wide = ToWide(text);
			DrawTextW(dc, wide.c_str(), -1, &rect,
				DT_CENTER | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

			SelectObject(dc, previousFont);
		}

		void Paint(const HDC target, const RECT& client)
		{
			//Status changes repaint the whole window, so a back buffer is what keeps it flicker free
			const HDC dc = CreateCompatibleDC(target);
			const HBITMAP buffer = CreateCompatibleBitmap(target, client.right, client.bottom);
			const auto previousBitmap = SelectObject(dc, buffer);

			const HBRUSH background = CreateSolidBrush(k_BackgroundColor);
			FillRect(dc, &client, background);
			DeleteObject(background);

			const HBRUSH border = CreateSolidBrush(k_BorderColor);
			FrameRect(dc, &client, border);
			DeleteObject(border);

			DrawLogo(dc, client);
			DrawTextLine(dc, s_Data.title, s_Data.titleFont, k_TitleColor, Scaled(k_TitleTop), client);
			DrawTextLine(dc, s_Data.status, s_Data.statusFont, k_StatusColor, Scaled(k_StatusTop), client);

			BitBlt(target, 0, 0, client.right, client.bottom, dc, 0, 0, SRCCOPY);

			SelectObject(dc, previousBitmap);
			DeleteObject(buffer);
			DeleteDC(dc);
		}

		LRESULT CALLBACK SplashScreenWndProc(HWND hwnd, const UINT message,
			const WPARAM wParam, const LPARAM lParam)
		{
			switch (message)
			{
			case WM_ERASEBKGND:
				return 1;
			case WM_PAINT:
			{
				PAINTSTRUCT paint;
				const HDC dc = BeginPaint(hwnd, &paint);

				RECT client;
				GetClientRect(hwnd, &client);
				Paint(dc, client);

				EndPaint(hwnd, &paint);
				return 0;
			}
			default:
				break;
			}

			return DefWindowProc(hwnd, message, wParam, lParam);
		}

		void PumpMessages()
		{
			//Only the splash screen messages are dispatched: the main window is already created by
			//then and its events are not expected before the game loop starts
			MSG message;
			while (PeekMessage(&message, s_Data.window, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
	}

	void SplashScreen::Show(const String& title, const Path& logoPath)
	{
		LEV_PROFILE_FUNCTION();

		if (s_Data.window != nullptr) return;

		s_Data.dpi = GetDpiForSystem();
		s_Data.title = title;
		s_Data.status = "Starting up";

		LoadLogo(logoPath);

		//The fonts have to exist before the window is shown, the first paint already uses them
		s_Data.titleFont = CreateSplashFont(16, FW_SEMIBOLD);
		s_Data.statusFont = CreateSplashFont(10, FW_NORMAL);

		const HINSTANCE instance = GetModuleHandle(nullptr);

		WNDCLASSEX windowClass{};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.lpfnWndProc = SplashScreenWndProc;
		windowClass.hInstance = instance;
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		windowClass.lpszClassName = k_WindowClassName;
		RegisterClassEx(&windowClass);

		const int width = Scaled(k_Width);
		const int height = Scaled(k_Height);
		const int posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		const int posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		//WS_EX_APPWINDOW keeps a taskbar button while the main window is still hidden
		s_Data.window = CreateWindowEx(WS_EX_APPWINDOW, k_WindowClassName, ToWide(title).c_str(),
			WS_POPUP, posX, posY, width, height, nullptr, nullptr, instance, nullptr);

		if (s_Data.window == nullptr)
		{
			Log::CoreWarning("Failed to create the splash screen window. Error code {0}", GetLastError());
			return;
		}

		//Windows 11 rounds the corners of the frameless window, older systems ignore the attribute
		DWORD cornerPreference = k_RoundedCorners;
		DwmSetWindowAttribute(s_Data.window, k_WindowCornerPreference, &cornerPreference, sizeof(cornerPreference));

		ShowWindow(s_Data.window, SW_SHOW);
		SetForegroundWindow(s_Data.window);
		UpdateWindow(s_Data.window);

		PumpMessages();
	}

	void SplashScreen::SetStatus(const String& status)
	{
		if (s_Data.window == nullptr) return;

		s_Data.status = status;

		InvalidateRect(s_Data.window, nullptr, FALSE);
		UpdateWindow(s_Data.window);

		PumpMessages();
	}

	void SplashScreen::Hide()
	{
		LEV_PROFILE_FUNCTION();

		if (s_Data.window == nullptr) return;

		//WM_DESTROY is sent, not posted, so nothing is left to pump for the splash screen
		DestroyWindow(s_Data.window);
		s_Data.window = nullptr;

		UnregisterClass(k_WindowClassName, GetModuleHandle(nullptr));

		if (s_Data.logo) DeleteObject(s_Data.logo);
		if (s_Data.titleFont) DeleteObject(s_Data.titleFont);
		if (s_Data.statusFont) DeleteObject(s_Data.statusFont);

		s_Data = SplashScreenData{};
	}

	bool SplashScreen::IsShown() { return s_Data.window != nullptr; }
}
