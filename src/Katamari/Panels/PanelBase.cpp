#include "pch.h"
#include "PanelBase.h"

#include <imgui.h>

namespace LevEngine::Editor
{
	void Panel::Render()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		m_Focused = ImGui::IsWindowFocused();
		m_Hovered = ImGui::IsWindowHovered();

		DrawContent();

		ImGui::End();
		ImGui::PopStyleVar();
	}
}
