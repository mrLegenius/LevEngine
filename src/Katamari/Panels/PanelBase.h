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

		void Focus() const { ImGui::FocusWindow(m_Window); }

	protected:
		virtual std::string GetName() = 0;
		virtual void DrawContent() = 0;

		ImGuiWindow* m_Window;

		bool m_Focused = false;
		bool m_Hovered = false;
	};
}

