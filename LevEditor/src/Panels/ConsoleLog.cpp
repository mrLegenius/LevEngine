#include "pch.h"
#include "ConsoleLog.h"

namespace LevEngine::Editor
{
    std::shared_ptr<ConsoleLog> ConsoleLog::s_Instance;

    ConsoleLog::ConsoleLog()
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

    void ConsoleLog::Init()
    {
        if (s_Instance) return;

        //spdlog uses shared_ptr so we use it here as well
        s_Instance = std::shared_ptr<ConsoleLog>(new ConsoleLog());
        Log::Logger::AddLogHandler(s_Instance);
    }

    const Vector<ConsoleLog::Item>& ConsoleLog::GetItems()
    {
        static Vector<Item> empty;
        return s_Instance ? s_Instance->m_Items : empty;
    }

    uint64_t ConsoleLog::GetRevision()
    {
        return s_Instance ? s_Instance->m_Revision : 0;
    }

    void ConsoleLog::Clear()
    {
        if (!s_Instance) return;

        s_Instance->m_Items.clear();
        ++s_Instance->m_Revision;
    }

    void ConsoleLog::sink_it_(const spdlog::details::log_msg& msg)
    {
        spdlog::memory_buf_t formatted;
        formatter_->format(msg, formatted);
        const auto& message = fmt::to_string(formatted);

        const auto color = m_Colors[msg.level];
        m_Items.push_back({ color, message.c_str() });
        ++m_Revision;
    }

    void ConsoleLog::flush_()
    {
    }
}
