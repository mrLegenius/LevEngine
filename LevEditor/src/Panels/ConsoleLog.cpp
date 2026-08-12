#include "pch.h"
#include "ConsoleLog.h"

namespace LevEngine::Editor
{
    std::shared_ptr<ConsoleLog> ConsoleLog::s_Instance;

    ConsoleLog::ConsoleLog()
    {
        set_pattern("[%H:%M:%S] %n: %v");
        //The panels filter by level themselves, so everything is kept
        set_level(spdlog::level::trace);

        m_Items.reserve(MaxItems);
    }

    void ConsoleLog::Init()
    {
        if (s_Instance) return;

        //spdlog uses shared_ptr so we use it here as well
        s_Instance = std::shared_ptr<ConsoleLog>(new ConsoleLog());
        Log::Logger::AddLogHandler(s_Instance);
    }

    ConsoleLog::ReadResult ConsoleLog::ReadItems(Vector<Item>& items, Cursor& cursor)
    {
        ReadResult result;

        if (!s_Instance)
        {
            if (!items.empty())
            {
                items.clear();
                result.wasReset = true;
            }
            return result;
        }

        std::lock_guard lock(s_Instance->mutex_);

        const auto& logItems = s_Instance->m_Items;
        const uint64_t logFirst = s_Instance->m_FirstIndex;
        const uint64_t logNext = logFirst + logItems.size();

        const bool wasCleared = cursor.generation != s_Instance->m_Generation;
        // The reader holds messages the log has already dropped
        const uint64_t dropped = wasCleared || logFirst <= cursor.firstIndex ? 0 : logFirst - cursor.firstIndex;

        if (wasCleared || dropped >= items.size())
        {
            if (!items.empty())
            {
                items.clear();
                result.wasReset = true;
            }
            cursor.firstIndex = logFirst;
        }
        else if (dropped > 0)
        {
            items.erase(items.begin(), items.begin() + dropped);
            cursor.firstIndex = logFirst;
            result.droppedCount = dropped;
        }

        cursor.generation = s_Instance->m_Generation;

        const uint64_t readerNext = cursor.firstIndex + items.size();
        if (readerNext < logNext)
        {
            const auto begin = logItems.begin() + (readerNext - logFirst);
            items.insert(items.end(), begin, logItems.end());
            result.appendedCount = static_cast<size_t>(logNext - readerNext);
        }

        return result;
    }

    void ConsoleLog::Clear()
    {
        if (!s_Instance) return;

        std::lock_guard lock(s_Instance->mutex_);

        s_Instance->m_FirstIndex += s_Instance->m_Items.size();
        s_Instance->m_Items.clear();
        ++s_Instance->m_Generation;
    }

    Color ConsoleLog::GetColor(const spdlog::level::level_enum level)
    {
        switch (level)
        {
        case spdlog::level::trace:
        case spdlog::level::debug:
            return Color::Aqua;
        case spdlog::level::warn:
            return Color::Yellow;
        case spdlog::level::err:
        case spdlog::level::critical:
            return Color::Red;
        default:
            return Color::White;
        }
    }

    void ConsoleLog::sink_it_(const spdlog::details::log_msg& msg)
    {
        spdlog::memory_buf_t formatted;
        formatter_->format(msg, formatted);

        // The pattern appends an end of line, which would render as an empty line
        size_t length = formatted.size();
        while (length > 0 && (formatted.data()[length - 1] == '\n' || formatted.data()[length - 1] == '\r'))
            --length;

        // Drop the oldest messages in batches to keep the memory bounded
        if (m_Items.size() >= MaxItems)
        {
            m_Items.erase(m_Items.begin(), m_Items.begin() + TrimCount);
            m_FirstIndex += TrimCount;
        }

        m_Items.push_back({ msg.level, String(formatted.data(), length) });
    }

    void ConsoleLog::flush_()
    {
    }
}
