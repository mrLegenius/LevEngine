#pragma once
#include "spdlog/spdlog.h"

//Able to log custom classes
#include "DataTypes/String.h"
#include "spdlog/fmt/ostr.h"

namespace LevEngine::Log
{
	class Logger
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetLogger() { return m_Logger; }

		static void AddLogHandler(const std::shared_ptr<spdlog::sinks::sink>& handler)
		{
			m_CoreLogger->sinks().push_back(handler);
			m_Logger->sinks().push_back(handler);
		}

	private:
		static std::shared_ptr<spdlog::logger> m_CoreLogger;
		static std::shared_ptr<spdlog::logger> m_Logger;
	};

#pragma region ***Engine logging***
	template <typename T> constexpr void CoreTrace(T message) { Logger::GetCoreLogger()->trace(message); }
	template <typename T> constexpr void CoreDebug(T message) { Logger::GetCoreLogger()->debug(message); }
	template <typename T> constexpr void CoreInfo(T message) { Logger::GetCoreLogger()->info(message); }
	template <typename T> constexpr void CoreWarning(T message) { Logger::GetCoreLogger()->warn(message); }
	template <typename T> constexpr void CoreError(T message) { Logger::GetCoreLogger()->error(message); }
	template <typename T> constexpr void CoreCritical(T message) { Logger::GetCoreLogger()->critical(message); }

	template<typename... Args> constexpr void CoreTrace(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetCoreLogger()->trace(format, std::forward<Args>(args)...);
	}
	template<typename... Args> constexpr void CoreDebug(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetCoreLogger()->debug(format, std::forward<Args>(args)...);
	}
	template<typename... Args> constexpr void CoreInfo(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetCoreLogger()->info(format, std::forward<Args>(args)...);
	}
	template<typename... Args> constexpr void CoreWarning(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetCoreLogger()->warn(format, std::forward<Args>(args)...);
	}
	template<typename... Args> constexpr void CoreError(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetCoreLogger()->error(format, std::forward<Args>(args)...);
	}
	template<typename... Args> constexpr void CoreCritical(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetCoreLogger()->critical(format, std::forward<Args>(args)...);
	}
#pragma endregion

#pragma region ***Client logging***
	template <typename T> constexpr void Trace(T message) { Logger::GetLogger()->trace(message); }
	template <typename T> constexpr void Debug(T message) { Logger::GetLogger()->debug(message); }
	template <typename T> constexpr void Info(T message) { Logger::GetLogger()->info(message); }
	template <typename T> constexpr void Warning(T message) { Logger::GetLogger()->warn(message); }
	template <typename T> constexpr void Error(T message) { Logger::GetLogger()->error(message); }
	template <typename T> constexpr void Critical(T message) { Logger::GetLogger()->critical(message); }

	template<typename... Args> constexpr void Trace(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetLogger()->trace(format, std::forward<Args>(args)...);
	}
	template<typename... Args> constexpr void Debug(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetLogger()->debug(format, std::forward<Args>(args)...);
	}
	template<typename... Args> constexpr void Info(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetLogger()->info(format, std::forward<Args>(args)...);
	}
	template<typename... Args> constexpr void Warning(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetLogger()->warn(format, std::forward<Args>(args)...);
	}
	template<typename... Args> constexpr void Error(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetLogger()->error(format, std::forward<Args>(args)...);
	}
	template<typename... Args> constexpr void Critical(fmt::format_string<Args...> format, Args&&...args)
	{
		Logger::GetLogger()->critical(format, std::forward<Args>(args)...);
	}
#pragma endregion 
}

namespace fmt
{
	template <> struct fmt::formatter<eastl::string> : formatter<std::string> {
		auto format(eastl::string c, format_context& ctx) {
			return formatter<std::string>::format(c.c_str(), ctx);
		}
	};
}