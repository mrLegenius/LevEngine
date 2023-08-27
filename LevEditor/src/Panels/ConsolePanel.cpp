#include "pch.h"
#include "ConsolePanel.h"

namespace LevEngine::Editor
{
	void ConsolePanel::DrawContent()
	{
		//<--- Top menu ---<<
		if (ImGui::SmallButton("Clear")) { ClearLog(); }

        ImGui::SameLine();
        m_Filter.Draw("Search", 180);
		ImGui::Separator();

		//<--- Output ---<<
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false);

        for (int i = 0; i < m_Items.size(); i++)
        {
            const auto& item = m_Items[i];
            const auto message = item.message.c_str();
            if (!m_Filter.PassFilter(message))
                continue;

            ImGui::BeginChild(i + 1, ImVec2(0, 30), true, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
            const auto color = item.color;
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ color.r, color.g, color.b, 1});
            ImGui::AlignTextToFramePadding();
            ImGui::Text(message);
            ImGui::PopStyleColor();
            ImGui::EndChild();
        }

        ImGui::EndChild();
	}
}
