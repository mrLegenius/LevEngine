#include "levpch.h"
#include "ImGuiLayer.h"

#include "imgui/imgui.h"
#include "Kernel/Application.h"

#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imguizmo/ImGuizmo.h"

#include <d3d11.h>
#include <tchar.h>
#include <wrl/client.h>

namespace LevEngine
{
	extern ID3D11DeviceContext* context;
	extern Microsoft::WRL::ComPtr<ID3D11Device> device;

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
	{
		
	}

	void ImGuiLayer::OnAttach()
	{
		LEV_PROFILE_FUNCTION();
		
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		io.Fonts->AddFontFromFileTTF("resources/Fonts/Roboto/Roboto-Bold.ttf", 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("resources/Fonts/Roboto/Roboto-Regular.ttf", 18.0f);
		
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		style.FrameRounding = 10;

		SetDarkThemeColors();

		const auto& app = Application::Get();
		const auto window = static_cast<HWND*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(device.Get(), context);
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		if(m_IsBlockingEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			event.handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}
	void ImGuiLayer::OnDetach()
	{
		LEV_PROFILE_FUNCTION();
		
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnGUIRender()
	{
	}

#define IntToFloat(x) ((x) / 255.0f)
	void ImGuiLayer::SetDarkThemeColors()
	{
		constexpr auto DarkBlue = ImVec4{ IntToFloat(21), IntToFloat(24), IntToFloat(31), 1.0f };
		constexpr auto Blue = ImVec4{ IntToFloat(39), IntToFloat(46), IntToFloat(54), 1.0f };
		constexpr auto Cyan = ImVec4{ IntToFloat(67), IntToFloat(217), IntToFloat(232), 1.0f };

		constexpr auto DarkOrange = ImVec4{ IntToFloat(207), IntToFloat(56), IntToFloat(25), 1.0f };
		constexpr auto LightOrange = ImVec4{ IntToFloat(235), IntToFloat(93), IntToFloat(19), 1.0f };
		constexpr auto WhiteOrange = ImVec4{ IntToFloat(231), IntToFloat(116), IntToFloat(62), 1.0f };

		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = DarkBlue;

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = LightOrange;
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = Blue;
		colors[ImGuiCol_FrameBgHovered] = WhiteOrange;
		colors[ImGuiCol_FrameBgActive] = DarkOrange;

		// Tabs
		colors[ImGuiCol_Tab] = LightOrange;
		colors[ImGuiCol_TabHovered] = LightOrange;
		colors[ImGuiCol_TabActive] = WhiteOrange;
		colors[ImGuiCol_TabUnfocused] = DarkOrange;
		colors[ImGuiCol_TabUnfocusedActive] = DarkOrange;

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	void ImGuiLayer::Begin()
	{
		LEV_PROFILE_FUNCTION();
		
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		LEV_PROFILE_FUNCTION();
		
		ImGuiIO& io = ImGui::GetIO();
		auto& app = Application::Get();
		auto& window = app.GetWindow();
		io.DisplaySize = ImVec2(static_cast<float>(window.GetWidth()), static_cast<float>(window.GetHeight()));

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void ImGuiLayer::BlockEvents(const bool value)
	{
		m_IsBlockingEvents = value;
	}
}
