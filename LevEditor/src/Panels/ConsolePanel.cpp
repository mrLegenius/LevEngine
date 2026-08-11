#include "pch.h"
#include "ConsolePanel.h"

#include "ConsoleLog.h"

namespace LevEngine::Editor
{
	void ConsolePanel::DrawContent()
	{
		LEV_PROFILE_FUNCTION();

		//<--- Top menu ---<<
		if (ImGui::SmallButton("Clear")) { ConsoleLog::Clear(); }

        ImGui::SameLine();
        m_Filter.Draw("Search", 180);

		ImGui::SameLine();
		ImGui::Checkbox("AutoScroll", &m_IsAutoScroll);

		ImGui::Separator();

		//<--- Output ---<<
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false);

		const auto& items = ConsoleLog::GetItems();
        for (int i = 0; i < items.size(); i++)
        {
            const auto& item = items[i];
            const auto message = item.message.c_str();
            if (!m_Filter.PassFilter(message))
                continue;

            ImGui::BeginChild(i + 1, ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_ResizeY | ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
            const auto color = item.color;
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ color.r, color.g, color.b, 1});
            ImGui::AlignTextToFramePadding();
        	ImGui::TextWrapped(message);
            ImGui::PopStyleColor();
            ImGui::EndChild();
        }

		const auto revision = ConsoleLog::GetRevision();
		const bool wasItemAdded = revision != m_LastRevision;
		m_LastRevision = revision;

		if (m_IsAutoScroll && (wasItemAdded || m_IsSecondFrameCheck))
		{
			m_IsSecondFrameCheck = wasItemAdded;
			ImGui::SetScrollHereY(1.0f);
		}

        ImGui::EndChild();
	}
}
