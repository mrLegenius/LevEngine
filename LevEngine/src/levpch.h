#pragma once

#include <stdint.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <cstdint>
#include <filesystem>

#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <set>

#include <imgui.h>
#include <imgui_internal.h>
#include <yaml-cpp/yaml.h>

#include "Debugging/Profiler.h"
#include "Kernel/Logger.h"

#include "Kernel/Utils.h"
#include "Kernel/Asserts.h"
#include "Kernel/PointerUtils.h"
#include "Kernel/UUID.h"

#ifdef LEV_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
