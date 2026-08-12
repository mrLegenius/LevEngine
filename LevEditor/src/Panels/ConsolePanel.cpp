#include "pch.h"
#include "ConsolePanel.h"

#include "ConsoleLog.h"
#include "GUI/Icons/IconsFontAwesome6.h"

#include <EASTL/algorithm.h>

namespace LevEngine::Editor
{
	namespace
	{
		constexpr uint32_t LevelBit(const spdlog::level::level_enum level)
		{
			return 1u << static_cast<int>(level);
		}

		//Errors and criticals share a toggle, they are the same thing to look for
		constexpr uint32_t k_ErrorLevels = LevelBit(spdlog::level::err) | LevelBit(spdlog::level::critical);

		struct LevelToggle
		{
			const char* icon;
			//Also the id of the button, the icon and the count are not stable enough
			const char* name;
			uint32_t levels;
		};

		constexpr LevelToggle k_LevelToggles[] = {
			{ ICON_FA_SHOE_PRINTS, "Trace", LevelBit(spdlog::level::trace) },
			{ ICON_FA_BUG, "Debug", LevelBit(spdlog::level::debug) },
			{ ICON_FA_CIRCLE_INFO, "Info", LevelBit(spdlog::level::info) },
			{ ICON_FA_TRIANGLE_EXCLAMATION, "Warning", LevelBit(spdlog::level::warn) },
			{ ICON_FA_CIRCLE_EXCLAMATION, "Error", k_ErrorLevels },
		};

		//The lowest level in the mask, its color represents the whole group
		spdlog::level::level_enum GetFirstLevel(const uint32_t levels)
		{
			for (int level = 0; level < spdlog::level::n_levels; ++level)
			{
				if (levels & (1u << level))
					return static_cast<spdlog::level::level_enum>(level);
			}

			return spdlog::level::info;
		}

		//Keeps the next item on the current row while it fits there. The top menu is
		//wider than the panel can get, so it has to be able to wrap
		void PlaceNextItem(const float width)
		{
			const auto& style = ImGui::GetStyle();
			const float nextItemEnd = ImGui::GetItemRectMax().x + style.ItemSpacing.x + width;
			const float rowEnd = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

			if (nextItemEnd <= rowEnd)
				ImGui::SameLine();
		}

		float GetButtonWidth(const char* label)
		{
			return ImGui::CalcTextSize(label, nullptr, true).x + ImGui::GetStyle().FramePadding.x * 2;
		}

		//Draws one toggle, flipping the levels it stands for in 'levelMask' when clicked
		void DrawLevelToggle(const char* label, const LevelToggle& toggle, const int count, uint32_t& levelMask)
		{
			const bool isShown = (levelMask & toggle.levels) != 0;

			const auto color = ConsoleLog::GetColor(GetFirstLevel(toggle.levels));
			const auto textColor = isShown
				? ImVec4{ color.r, color.g, color.b, 1 }
				: ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
			//Hidden levels read as a flat dimmed icon, shown ones as a normal button
			const auto buttonColor = isShown ? ImGui::GetStyleColorVec4(ImGuiCol_Button) : ImVec4{ 0, 0, 0, 0 };

			ImGui::PushStyleColor(ImGuiCol_Text, textColor);
			ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);

			if (ImGui::SmallButton(label))
				levelMask ^= toggle.levels;

			ImGui::PopStyleColor(2);

			//The icons say little on their own, so the name is in the tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("%s: %d messages\nClick to %s them",
					toggle.name, count, isShown ? "hide" : "show");
			}
		}
	}

	void ConsolePanel::DrawContent()
	{
		LEV_PROFILE_FUNCTION();

		//<--- Top menu ---<<
		if (ImGui::SmallButton("Clear")) { ConsoleLog::Clear(); }

        ImGui::SameLine();
        m_Filter.Draw("Search", 180);

		ImGui::SameLine();
		ImGui::Checkbox("AutoScroll", &m_IsAutoScroll);

		//Messages the logger had to throw away to keep up with the logging
		if (const auto dropped = Log::Logger::GetDroppedMessageCount(); dropped > 0)
		{
			const auto text = Format("{} dropped", dropped);
			const auto color = ConsoleLog::GetColor(spdlog::level::warn);
			PlaceNextItem(ImGui::CalcTextSize(text.c_str()).x);
			ImGui::TextColored(ImVec4{ color.r, color.g, color.b, 1 }, "%s", text.c_str());
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Messages were logged faster than they could be written");
		}

		DrawLevelToggles();

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

			// Only the messages inside the view are submitted to ImGui
			const int first = FindFirstVisible(viewTop);
			ImGui::SetCursorPosY(contentStart + m_Offsets[first]);

			for (int i = first; i < count && m_Offsets[i] < viewBottom; ++i)
			{
				const auto& item = m_Items[m_Visible[i]];
				const float height = m_Heights[m_Visible[i]];

				const auto color = ConsoleLog::GetColor(item.level);
				const ImVec2 topLeft = ImGui::GetCursorScreenPos();
				const ImVec2 bottomRight{ topLeft.x + regionWidth, topLeft.y + height };

				// Warnings and worse get a plate, so they can be spotted while scrolling
				if (item.level >= spdlog::level::warn)
				{
					drawList->AddRectFilled(topLeft, bottomRight,
						ImGui::GetColorU32(ImVec4{ color.r, color.g, color.b, 0.08f }), style.ChildRounding);
				}

				drawList->AddRect(topLeft, bottomRight,
					ImGui::GetColorU32(ImVec4{ color.r, color.g, color.b, 0.25f }), style.ChildRounding);

				ImGui::SetCursorScreenPos(ImVec2{ topLeft.x + style.FramePadding.x, topLeft.y + style.FramePadding.y });
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

	void ConsolePanel::DrawLevelToggles()
	{
		const auto& style = ImGui::GetStyle();

		//The labels are needed twice, once to measure the row and once to draw it
		String labels[eastl::size(k_LevelToggles)];
		float rowWidth = 0;

		for (size_t i = 0; i < eastl::size(k_LevelToggles); ++i)
		{
			const auto& toggle = k_LevelToggles[i];
			labels[i] = Format("{} {}##Level{}", toggle.icon, GetLevelCount(toggle.levels), toggle.name);
			rowWidth += GetButtonWidth(labels[i].c_str()) + style.ItemSpacing.x;
		}
		rowWidth -= style.ItemSpacing.x;

		//The toggles sit at the right end of the menu, on the row above when they fit
		const float rowStart = ImGui::GetWindowContentRegionMax().x - rowWidth;
		const float previousItemEnd = ImGui::GetItemRectMax().x - ImGui::GetWindowPos().x;

		if (rowStart > previousItemEnd + style.ItemSpacing.x)
		{
			ImGui::SameLine(rowStart);
		}
		else
		{
			ImGui::SetCursorPosX(ImMax(rowStart, ImGui::GetWindowContentRegionMin().x));
		}

		for (size_t i = 0; i < eastl::size(k_LevelToggles); ++i)
		{
			if (i > 0) ImGui::SameLine();

			const auto& toggle = k_LevelToggles[i];
			DrawLevelToggle(labels[i].c_str(), toggle, GetLevelCount(toggle.levels), m_LevelMask);
		}
	}

	int ConsolePanel::GetLevelCount(const uint32_t levels) const
	{
		int count = 0;
		for (int level = 0; level < spdlog::level::n_levels; ++level)
		{
			if (levels & (1u << level))
				count += m_LevelCounts[level];
		}

		return count;
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

		if (m_LastLevelMask != m_LevelMask)
		{
			m_LastLevelMask = m_LevelMask;
			rebuildVisible = true;
		}

		if (read.wasReset || read.droppedCount > 0)
		{
			//The dropped messages are gone, so their levels are no longer known
			eastl::fill(eastl::begin(m_LevelCounts), eastl::end(m_LevelCounts), 0);
			for (const auto& item : m_Items)
				++m_LevelCounts[item.level];
		}
		else
		{
			for (size_t i = m_Items.size() - read.appendedCount; i < m_Items.size(); ++i)
				++m_LevelCounts[m_Items[i].level];
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
			const auto& item = m_Items[i];
			if (!(m_LevelMask & (1u << item.level)))
				continue;

			const auto& message = item.message;
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
