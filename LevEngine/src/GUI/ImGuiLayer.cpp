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

#include "Kernel/Window.h"

#include "Icons/IconsFontAwesome6.h"

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

		io.Fonts->AddFontFromFileTTF("LevResources/Fonts/Ubuntu/Ubuntu-Bold.ttf", 12.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("LevResources/Fonts/Ubuntu/Ubuntu-Regular.ttf", 12.0f);

		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
		static constexpr ImWchar IconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		io.Fonts->AddFontFromFileTTF("LevResources/Fonts/Icons/fa-solid-900.ttf", 12.0f, &config, IconRanges);

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		//Spacing
		style.ItemSpacing = ImVec2{ 5.0f, 2.0f };
		style.ItemInnerSpacing = ImVec2{ 5.0f, 5.0f };
		style.FramePadding = ImVec2{ 5.0f, 2.0f };

		//Rounding
		style.FrameRounding = 2;
		style.GrabRounding = 2;
		style.TabRounding = 2;
		style.PopupRounding = 2;
		style.ChildRounding = 2;
		style.ScrollbarRounding = 2;

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
		constexpr auto darkBackground = ImVec4{ IntToFloat(25), IntToFloat(25), IntToFloat(25), 1.0f };
		constexpr auto lightBackground = ImVec4{ IntToFloat(40), IntToFloat(40), IntToFloat(40), 1.0f };

		constexpr auto elementDark = ImVec4{ IntToFloat(77), IntToFloat(51), IntToFloat(11), 1.0f };
		constexpr auto elementRegular = ImVec4{ IntToFloat(140), IntToFloat(94), IntToFloat(20), 1.0f };
		constexpr auto elementLight = ImVec4{ IntToFloat(204), IntToFloat(137), IntToFloat(29), 1.0f };
		constexpr auto white = ImVec4{ IntToFloat(255), IntToFloat(255), IntToFloat(255), 1.0f };
		constexpr auto gray = ImVec4{ IntToFloat(128), IntToFloat(128), IntToFloat(128), 1.0f };

		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = lightBackground;
		colors[ImGuiCol_ChildBg] = lightBackground;
		colors[ImGuiCol_PopupBg] = lightBackground;

		colors[ImGuiCol_Border] = darkBackground;
		colors[ImGuiCol_BorderShadow] = darkBackground;

		colors[ImGuiCol_MenuBarBg] = darkBackground;

		// Resize grip in lower-right and lower-left corners of windows.
		colors[ImGuiCol_ResizeGrip] = elementDark;
		colors[ImGuiCol_ResizeGripHovered] = elementRegular;
		colors[ImGuiCol_ResizeGripActive] = elementLight;

		// Title
		colors[ImGuiCol_TitleBg] = darkBackground;
		colors[ImGuiCol_TitleBgActive] = darkBackground;
		colors[ImGuiCol_TitleBgCollapsed] = darkBackground;

		//Scrollbar
		colors[ImGuiCol_ScrollbarBg] = darkBackground;
		colors[ImGuiCol_ScrollbarGrab] = elementDark;
		colors[ImGuiCol_ScrollbarGrabHovered] = elementRegular;
		colors[ImGuiCol_ScrollbarGrabActive] = elementLight;

		// Header colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
		colors[ImGuiCol_Header] = elementRegular;
		colors[ImGuiCol_HeaderHovered] = elementLight;
		colors[ImGuiCol_HeaderActive] = elementLight;

		// Buttons
		colors[ImGuiCol_Button] = elementDark;
		colors[ImGuiCol_ButtonHovered] = elementRegular;
		colors[ImGuiCol_ButtonActive] = elementLight;

		// Background of checkbox, radio button, plot, slider, text input
		colors[ImGuiCol_FrameBg] = darkBackground;
		colors[ImGuiCol_FrameBgHovered] = elementDark;
		colors[ImGuiCol_FrameBgActive] = elementRegular;

		colors[ImGuiCol_CheckMark] = white;
		colors[ImGuiCol_SliderGrab] = elementRegular;
		colors[ImGuiCol_SliderGrabActive] = elementLight;

		//Separator
		colors[ImGuiCol_Separator] = elementDark;
		colors[ImGuiCol_SeparatorHovered] = elementRegular;
		colors[ImGuiCol_SeparatorActive] = elementLight;

		// Tabs
		colors[ImGuiCol_Tab] = elementDark;
		colors[ImGuiCol_TabHovered] = elementRegular;
		colors[ImGuiCol_TabActive] = elementLight;
		colors[ImGuiCol_TabUnfocused] = elementDark;
		colors[ImGuiCol_TabUnfocusedActive] = elementRegular;

		//Docking
		colors[ImGuiCol_DockingPreview] = elementDark;
		colors[ImGuiCol_DockingEmptyBg] = darkBackground;

		//Plots
		colors[ImGuiCol_PlotLines] = elementDark;
		colors[ImGuiCol_PlotLinesHovered] = elementRegular;
		colors[ImGuiCol_PlotHistogram] = elementDark;
		colors[ImGuiCol_PlotHistogramHovered] = elementRegular;

		//Tables
		colors[ImGuiCol_TableHeaderBg] = darkBackground;
		colors[ImGuiCol_TableBorderStrong] = elementRegular;
		colors[ImGuiCol_TableBorderLight] = elementLight;
		colors[ImGuiCol_TableRowBg] = elementDark;
		colors[ImGuiCol_TableRowBgAlt] = elementDark;

		//Drag Drop Target
		colors[ImGuiCol_DragDropTarget] = elementLight;

		//Text
		colors[ImGuiCol_Text] = white;
		colors[ImGuiCol_TextDisabled] = gray;
		colors[ImGuiCol_TextSelectedBg] = elementDark;
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
