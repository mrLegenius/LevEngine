#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include "src/Kernel/PointerUtils.h"
#include "src/Kernel/Logger.h"
#include "src/Kernel/Asserts.h"
#include "src/Debug/Profiler.h"

#ifdef LEV_PLATFORM_WINDOWS
#include <Windows.h>
#endif
