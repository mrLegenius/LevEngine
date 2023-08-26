#include "pch.h"
#include "PanelBase.h"

#include <imgui.h>

namespace LevEngine::Editor
{
	void Panel::Render()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_WindowPadding.x, m_WindowPadding.y));
		m_Active = ImGui::Begin(GetName().c_str());
		if (m_Active)
		{
			m_Window = ImGui::GetCurrentWindow();
			m_Focused = ImGui::IsWindowFocused();
			m_Hovered = ImGui::IsWindowHovered();

			DrawContent();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
}
