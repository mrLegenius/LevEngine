#pragma once

#ifdef LEV_DEBUG
#define LEV_ENABLE_ASSERTS 1
#endif

#if LEV_ENABLE_ASSERTS
#define LEV_ASSERT(x, ...) { if(!(x)) { Log::Error("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define LEV_CORE_ASSERT(x, ...) { if(!(x)) { Log::CoreError("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define LEV_THROW(...) { Log::CoreError("Exception: {0}", __VA_ARGS__); throw std::exception(); }
#define LEV_NOT_IMPLEMENTED LEV_THROW("Not implemented")

//TODO: this made for formatting working. But debug must be break on assert calling line instead of this function
template<typename Condition, typename FormatString, typename... Args>
constexpr void Assert(Condition condition, FormatString& format, Args&&...args)
{
	if (condition) return;

	std::string title = "Assertion Failed: ";
	title.append(format);

	LevEngine::Log::Error(title, std::forward<Args>(args)...);
	throw std::exception();
}

#else
#define LEV_ASSERT(x, ...)
#define LEV_CORE_ASSERT(x, ...)
#define LEV_NOT_IMPLEMENTED
#define LEV_THROW
#endif
