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
		// Recalculates the cached message heights and the filtered message list.
		// Returns true when the visible list changed
		bool UpdateLayout(float wrapWidth, const ConsoleLog::ReadResult& read);
		// Index of the first visible message at the given scroll offset
		[[nodiscard]] int FindFirstVisible(float viewTop) const;

		ImGuiTextFilter m_Filter;
		bool m_IsAutoScroll = true;
		bool m_IsSecondFrameCheck = false;

		// Panel side mirror of the log, so drawing does not lock the sink
		Vector<ConsoleLog::Item> m_Items;
		ConsoleLog::Cursor m_Cursor;
		// Height of every message in m_Items, measured for m_WrapWidth
		Vector<float> m_Heights;
		// Indices of the messages passing the filter
		Vector<int> m_Visible;
		// Y offset of every visible message, plus the total content height at the back
		Vector<float> m_Offsets;
		float m_WrapWidth = 0;
		float m_ItemSpacing = 0;
		String m_FilterText;
	};
}
