#pragma once
#include "Panel.h"
#include "spdlog/sinks/base_sink.h"

namespace LevEngine::Editor
{
	class ConsolePanel : public Panel, public spdlog::sinks::base_sink<std::mutex>
	{
	public:
		ConsolePanel();

	protected:
		String GetName() override { return "Console"; }
		void DrawContent() override;

	private:
		void ClearLog();

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override;

		void flush_() override;

	private:
		struct Item
		{
			Color color;
			String message;
		};
		Vector<Item> m_Items;
		UnorderedMap<spdlog::level::level_enum, Color> m_Colors;
		ImGuiTextFilter m_Filter;
		bool m_IsAutoScroll;
		bool m_WasItemAdded;
		bool m_IsSecondFrameCheck;
	};
}
