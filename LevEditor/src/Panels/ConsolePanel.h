#pragma once
#include "Panel.h"

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
		ImGuiTextFilter m_Filter;
		bool m_IsAutoScroll = true;
		uint64_t m_LastRevision = 0;
		bool m_IsSecondFrameCheck = false;
	};
}
