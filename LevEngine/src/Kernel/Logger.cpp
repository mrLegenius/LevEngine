#include "levpch.h"
#include "Logger.h"

#include "spdlog/async.h"
#include "spdlog/sinks/dist_sink.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace LevEngine::Log
{
	std::shared_ptr<spdlog::logger> Logger::m_CoreLogger;
	std::shared_ptr<spdlog::logger> Logger::m_Logger;

	namespace
	{
		constexpr char k_Pattern[] = "%^[%T] %n: %v%$";
		//Messages waiting to be written. A flood beyond that drops the oldest ones
		//instead of making the logging thread wait for the sinks
		constexpr size_t k_QueueSize = 16384;
		constexpr size_t k_WriterThreadCount = 1;

		//Both loggers share the sinks, so a handler is attached once and the logging
		//thread never sees the sink list change under it
		std::shared_ptr<spdlog::sinks::dist_sink_mt> s_Sinks;

		std::shared_ptr<spdlog::logger> CreateLogger(const char* name)
		{
			auto logger = std::make_shared<spdlog::async_logger>(name, s_Sinks, spdlog::thread_pool(),
				spdlog::async_overflow_policy::overrun_oldest);

			logger->set_pattern(k_Pattern);
			//TODO: make level changeable
			logger->set_level(spdlog::level::trace);
			//Errors are the ones worth having on disk when the process goes down
			logger->flush_on(spdlog::level::err);

			spdlog::register_logger(logger);

			return logger;
		}
	}

	void Logger::Init()
	{
		if (m_CoreLogger) return;

		spdlog::init_thread_pool(k_QueueSize, k_WriterThreadCount);

		s_Sinks = std::make_shared<spdlog::sinks::dist_sink_mt>();
		s_Sinks->add_sink(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

		m_CoreLogger = CreateLogger("LevEngine");
		m_Logger = CreateLogger("APP");
	}

	void Logger::Shutdown()
	{
		if (!m_CoreLogger) return;

		//Writes out everything that is still queued and stops the logging thread
		spdlog::shutdown();

		//Anything logged from here on has nowhere to go, but it should not crash
		const auto nullSink = std::make_shared<spdlog::sinks::null_sink_mt>();
		m_CoreLogger = std::make_shared<spdlog::logger>("LevEngine", nullSink);
		m_Logger = std::make_shared<spdlog::logger>("APP", nullSink);
		s_Sinks.reset();
	}

	void Logger::SetLevel(const spdlog::level::level_enum level)
	{
		if (m_CoreLogger) m_CoreLogger->set_level(level);
		if (m_Logger) m_Logger->set_level(level);
	}

	size_t Logger::GetDroppedMessageCount()
	{
		const auto pool = spdlog::thread_pool();
		return pool ? pool->overrun_counter() : 0;
	}

	void Logger::AddLogHandler(const std::shared_ptr<spdlog::sinks::sink>& handler)
	{
		if (!s_Sinks) return;

		s_Sinks->add_sink(handler);
	}
}
