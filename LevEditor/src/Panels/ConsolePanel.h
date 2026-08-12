#pragma once
#include "Panel.h"
#include "ConsoleLog.h"

namespace LevEngine::Editor
{
	class ConsolePanel final : public Panel
	{
	public:
		ConsolePanel() { m_DefaultWindowSize = Vector2{ 890, 390 }; }

	protected:
		String GetName() override { return "Console"; }
		void DrawContent() override;

	private:
		// Draws a button toggling the messages of the given levels, 'levels' being
		// a mask of spdlog level bits
		void DrawLevelToggle(const char* label, uint32_t levels);
		// Recalculates the cached message heights and the filtered message list.
		// Returns true when the visible list changed
		bool UpdateLayout(float wrapWidth, const ConsoleLog::ReadResult& read);
		// Index of the first visible message at the given scroll offset
		[[nodiscard]] int FindFirstVisible(float viewTop) const;

		ImGuiTextFilter m_Filter;
		bool m_IsAutoScroll = true;
		bool m_IsSecondFrameCheck = false;

		// Levels the panel shows. Trace and debug are hidden by default, they are
		// noise unless they are what is being looked for
		uint32_t m_LevelMask = ~((1u << spdlog::level::trace) | (1u << spdlog::level::debug));
		uint32_t m_LastLevelMask = m_LevelMask;
		// Messages per level currently held, shown on the level toggles
		int m_LevelCounts[spdlog::level::n_levels]{};

		// Panel side mirror of the log, so drawing does not lock the sink
		Vector<ConsoleLog::Item> m_Items;
		ConsoleLog::Cursor m_Cursor;
		// Height of every message in m_Items, measured for m_WrapWidth
		Vector<float> m_Heights;
		// Indices of the messages passing the filters
		Vector<int> m_Visible;
		// Y offset of every visible message, plus the total content height at the back
		Vector<float> m_Offsets;
		float m_WrapWidth = 0;
		float m_ItemSpacing = 0;
		String m_FilterText;
	};
}
