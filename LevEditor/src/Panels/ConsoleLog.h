#pragma once
#include "spdlog/sinks/base_sink.h"

namespace LevEngine::Editor
{
    // Single log sink shared by all console panels
    class ConsoleLog final : public spdlog::sinks::base_sink<std::mutex>
    {
    public:
        struct Item
        {
            Color color;
            String message;
        };

        // Reader position into the log. Panels keep one and pass it to ReadItems
        struct Cursor
        {
            // Global index of the first message the reader holds
            uint64_t firstIndex = 0;
            // Bumped by Clear, so readers can drop everything they hold
            uint64_t generation = 0;
        };

        struct ReadResult
        {
            // Messages dropped from the front of the reader's storage
            size_t droppedCount = 0;
            // Messages appended to the back of the reader's storage
            size_t appendedCount = 0;
            // The reader's storage was emptied, everything it cached is invalid
            bool wasReset = false;
        };

        // Messages kept in memory. The oldest ones are dropped beyond that
        static constexpr size_t MaxItems = 4096;
        // Number of messages dropped at once when the limit is reached, so the
        // dropping is amortized instead of happening on every message
        static constexpr size_t TrimCount = MaxItems / 4;

        //Creates the sink and attaches it to the logger
        static void Init();

        // Mirrors the log into 'items' by dropping and appending only what changed,
        // so panels can cache per message data instead of rebuilding it every frame
        static ReadResult ReadItems(Vector<Item>& items, Cursor& cursor);
        static void Clear();

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override;
        void flush_() override;

    private:
        ConsoleLog();

        static Color GetColor(spdlog::level::level_enum level);

        static std::shared_ptr<ConsoleLog> s_Instance;

        // Guarded by base_sink::mutex_
        Vector<Item> m_Items;
        // Global index of m_Items front. Never resets, so readers detect dropped messages
        uint64_t m_FirstIndex{};
        uint64_t m_Generation{};
    };
}
