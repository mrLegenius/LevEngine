﻿#pragma once
#include "Panel.h"
#include "spdlog/sinks/base_sink.h"

namespace LevEngine::Editor
{
	class ConsolePanel : public Panel, public spdlog::sinks::base_sink<std::mutex>
	{
	public:
		ConsolePanel()
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
	protected:
		String GetName() override { return "Console"; }
		void DrawContent() override;

	private:
		void ClearLog()
		{
			m_Items.clear();
		}

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			spdlog::memory_buf_t formatted;
			formatter_->format(msg, formatted);
			const auto& message = fmt::to_string(formatted);

			const auto color = m_Colors[msg.level];
			m_Items.push_back({color, message.c_str()});
		}
		void flush_() override
		{
		}

	private:
		struct Item
		{
			Color color;
			String message;
		};
		Vector<Item> m_Items;
		UnorderedMap<spdlog::level::level_enum, Color> m_Colors;
		ImGuiTextFilter m_Filter;
	};
}
