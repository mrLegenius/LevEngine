#include "pch.h"
#include "ConsolePanel.h"

namespace LevEngine::Editor
{
	ConsolePanel::ConsolePanel():
		m_IsAutoScroll(true),
		m_WasItemAdded(false)
	{
		set_pattern("[%H:%M:%S] %n: %v");
		set_level(spdlog::level::info);

		m_Colors.emplace(spdlog::level::trace, Color::Aqua);
		m_Colors.emplace(spdlog::level::debug, Color::Aqua);
		m_Colors.emplace(spdlog::level::info, Color::White);
		m_Colors.emplace(spdlog::level::warn, Color::Yellow);
		m_Colors.emplace(spdlog::level::err, Color::Red);
		m_Colors.emplace(spdlog::level::critical, Color::Red);
	}

	void ConsolePanel::DrawContent()
	{
		//<--- Top menu ---<<
		if (ImGui::SmallButton("Clear")) { ClearLog(); }

        ImGui::SameLine();
        m_Filter.Draw("Search", 180);
		
		ImGui::SameLine();
		ImGui::Checkbox("AutoScroll", &m_IsAutoScroll);
		
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

            ImGui::BeginChild(i + 1, ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_ResizeY | ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
            const auto color = item.color;
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ color.r, color.g, color.b, 1});
            ImGui::AlignTextToFramePadding();
        	ImGui::TextWrapped(message);
            ImGui::PopStyleColor();
            ImGui::EndChild();
        }

		if (m_IsAutoScroll && (m_WasItemAdded || m_IsSecondFrameCheck))
		{
			m_IsSecondFrameCheck = m_WasItemAdded;
			m_WasItemAdded = false;
			ImGui::SetScrollHereY(1.0f);
		}
		
        ImGui::EndChild();
		
	}

	void ConsolePanel::ClearLog()
	{
		m_Items.clear();
	}

	void ConsolePanel::sink_it_(const spdlog::details::log_msg& msg)
	{
		spdlog::memory_buf_t formatted;
		formatter_->format(msg, formatted);
		const auto& message = fmt::to_string(formatted);

		const auto color = m_Colors[msg.level];
		m_Items.push_back({color, message.c_str()});
		m_WasItemAdded = true;
	}

	void ConsolePanel::flush_()
	{
	}
}
