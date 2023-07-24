#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include "Debugging/Profiler.h"
#include "Kernel/Logger.h"
#include "Kernel/Asserts.h"

#ifdef LEV_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
