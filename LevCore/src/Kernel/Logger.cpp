#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/sinks/basic_file_sink.h>

namespace LevEngine::Log
{
	std::shared_ptr<spdlog::logger> Logger::m_CoreLogger;
	std::shared_ptr<spdlog::logger> Logger::m_Logger;

	void Logger::Init()
	{
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("LevEngine.log", true));

        logSinks[0]->set_pattern("%^[%T] %n: %v%$");
        logSinks[1]->set_pattern("[%T] [%l] %n: %v");

        m_CoreLogger = std::make_shared<spdlog::logger>("LevEngine", begin(logSinks), end(logSinks));
        spdlog::register_logger(m_CoreLogger);
        m_CoreLogger->set_level(spdlog::level::trace);
        m_CoreLogger->flush_on(spdlog::level::trace);

        m_Logger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
        spdlog::register_logger(m_Logger);
        m_Logger->set_level(spdlog::level::trace);
        m_Logger->flush_on(spdlog::level::trace);
    }
}
