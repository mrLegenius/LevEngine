#pragma once
#include <imgui.h>
#include <imgui_internal.h>

namespace LevEngine::Editor
{
	class Panel
	{
	public:
		virtual ~Panel() = default;
		void Render();

		[[nodiscard]] bool IsFocused() const { return m_Focused; }
		[[nodiscard]] bool IsHovered() const { return m_Hovered; }
		[[nodiscard]] bool IsActive() const { return m_Active; }

		void Focus() const { ImGui::FocusWindow(m_Window); }

		virtual bool OnKeyPressed(KeyPressedEvent& e) { return false; }

	protected:
		virtual std::string GetName() = 0;
		virtual void DrawContent() = 0;

		ImGuiWindow* m_Window{};

		bool m_Focused = false;
		bool m_Hovered = false;
		bool m_Active = false;

		Vector2 m_WindowPadding{ 10, 5};
	};
}

