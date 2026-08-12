#include "pch.h"
#include "ConsolePanel.h"

#include "ConsoleLog.h"

#include <EASTL/algorithm.h>

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

		const auto read = ConsoleLog::ReadItems(m_Items, m_Cursor);

		const auto& style = ImGui::GetStyle();
		const float regionWidth = ImGui::GetContentRegionAvail().x;
		const float wrapWidth = ImMax(regionWidth - style.FramePadding.x * 2, 1.0f);

		UpdateLayout(wrapWidth, read);

		const bool wasItemAdded = read.appendedCount > 0;
		const bool scrollToBottom = m_IsAutoScroll && (wasItemAdded || m_IsSecondFrameCheck);
		if (scrollToBottom)
			m_IsSecondFrameCheck = wasItemAdded;

		const int count = static_cast<int>(m_Visible.size());
		if (count > 0)
		{
			const float contentStart = ImGui::GetCursorPosY();
			const float totalHeight = m_Offsets.back();
			const float viewHeight = ImGui::GetWindowHeight();
			// The scroll set below is only applied on the next frame, so the messages
			// it is going to reveal are the ones that have to be drawn already
			const float viewTop = scrollToBottom ? ImMax(totalHeight - viewHeight, 0.0f) : ImGui::GetScrollY();
			const float viewBottom = viewTop + viewHeight;

			const auto drawList = ImGui::GetWindowDrawList();
			const auto borderColor = ImGui::GetColorU32(ImGuiCol_Border);

			// Only the messages inside the view are submitted to ImGui
			const int first = FindFirstVisible(viewTop);
			ImGui::SetCursorPosY(contentStart + m_Offsets[first]);

			for (int i = first; i < count && m_Offsets[i] < viewBottom; ++i)
			{
				const auto& item = m_Items[m_Visible[i]];
				const float height = m_Heights[m_Visible[i]];

				const ImVec2 topLeft = ImGui::GetCursorScreenPos();
				drawList->AddRect(topLeft, ImVec2{ topLeft.x + regionWidth, topLeft.y + height },
					borderColor, style.ChildRounding);

				ImGui::SetCursorScreenPos(ImVec2{ topLeft.x + style.FramePadding.x, topLeft.y + style.FramePadding.y });
				const auto color = item.color;
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ color.r, color.g, color.b, 1 });
				ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + wrapWidth);
				ImGui::TextUnformatted(item.message.c_str(), item.message.c_str() + item.message.size());
				ImGui::PopTextWrapPos();
				ImGui::PopStyleColor();

				ImGui::SetCursorScreenPos(ImVec2{ topLeft.x, topLeft.y + height + style.ItemSpacing.y });
			}

			// Claim the height of the skipped messages, so the scrolling range stays the same
			ImGui::SetCursorPosY(contentStart + ImMax(totalHeight - style.ItemSpacing.y, 0.0f));
			ImGui::Dummy(ImVec2{ 0, 0 });
		}

		if (scrollToBottom)
			ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
	}

	bool ConsolePanel::UpdateLayout(const float wrapWidth, const ConsoleLog::ReadResult& read)
	{
		LEV_PROFILE_FUNCTION();

		const auto& style = ImGui::GetStyle();
		const float spacing = style.ItemSpacing.y;

		if (m_Offsets.empty())
			m_Offsets.push_back(0.0f);

		bool rebuildVisible = false;

		if (read.wasReset)
		{
			m_Heights.clear();
			rebuildVisible = true;
		}
		else if (read.droppedCount > 0)
		{
			m_Heights.erase(m_Heights.begin(), m_Heights.begin() + read.droppedCount);
			// Every cached index shifted, so the filtered list is rebuilt
			rebuildVisible = true;
		}

		// Wrapping is what defines the height, so a resize invalidates every measurement
		if (ImFabs(wrapWidth - m_WrapWidth) > 1.0f || spacing != m_ItemSpacing)
		{
			m_WrapWidth = wrapWidth;
			m_ItemSpacing = spacing;
			m_Heights.clear();
			rebuildVisible = true;
		}

		if (m_FilterText != m_Filter.InputBuf)
		{
			m_FilterText = m_Filter.InputBuf;
			rebuildVisible = true;
		}

		// Measuring the text is the expensive part, so it is done once per message
		for (size_t i = m_Heights.size(); i < m_Items.size(); ++i)
		{
			const auto& message = m_Items[i].message;
			const float textHeight = ImGui::CalcTextSize(message.c_str(), message.c_str() + message.size(),
				false, wrapWidth).y;
			m_Heights.push_back(textHeight + style.FramePadding.y * 2);
		}

		if (!rebuildVisible && read.appendedCount == 0)
			return false;

		size_t firstToFilter = 0;
		if (rebuildVisible)
		{
			m_Visible.clear();
			m_Offsets.clear();
			m_Offsets.push_back(0.0f);
		}
		else
		{
			firstToFilter = m_Items.size() - read.appendedCount;
		}

		for (size_t i = firstToFilter; i < m_Items.size(); ++i)
		{
			const auto& message = m_Items[i].message;
			if (!m_Filter.PassFilter(message.c_str(), message.c_str() + message.size()))
				continue;

			m_Visible.push_back(static_cast<int>(i));
			m_Offsets.push_back(m_Offsets.back() + m_Heights[i] + spacing);
		}

		return true;
	}

	int ConsolePanel::FindFirstVisible(const float viewTop) const
	{
		const int count = static_cast<int>(m_Visible.size());
		if (count <= 0) return 0;

		// The offsets are sorted, so the message covering the top of the view is
		// the one right before the first offset below it
		const auto it = eastl::upper_bound(m_Offsets.begin(), m_Offsets.begin() + count, viewTop);
		const int index = static_cast<int>(it - m_Offsets.begin()) - 1;

		if (index < 0) return 0;
		return index < count ? index : count - 1;
	}
}
