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

        //Creates the sink and attaches it to the logger
        static void Init();

        [[nodiscard]] static const Vector<Item>& GetItems();
        //Incremented for every added message, so panels can detect new messages
        [[nodiscard]] static uint64_t GetRevision();
        static void Clear();

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override;
        void flush_() override;

    private:
        ConsoleLog();

        static std::shared_ptr<ConsoleLog> s_Instance;

        Vector<Item> m_Items;
        UnorderedMap<spdlog::level::level_enum, Color> m_Colors;
        uint64_t m_Revision{};
    };
}
