#pragma once
#include "Kernel/Logger.h"

#ifdef LEV_DEBUG
#define LEV_ENABLE_ASSERTS 1
#endif

#if LEV_ENABLE_ASSERTS
#define LEV_ASSERT(x, ...) { if(!(x)) { Log::Error("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define LEV_CORE_ASSERT(x, ...) { if(!(x)) { Log::CoreError("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define LEV_THROW(...) { Log::CoreError("Exception: {0}", __VA_ARGS__); throw std::exception(); }
#define LEV_NOT_IMPLEMENTED LEV_THROW("Not implemented")
#else
#define LEV_ASSERT(x, ...)
#define LEV_CORE_ASSERT(x, ...)
#define LEV_NOT_IMPLEMENTED
#endif
