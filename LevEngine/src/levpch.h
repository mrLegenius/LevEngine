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
#include <chrono>

#include <string>
#include <ostream>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <queue>
#include <unordered_set>
#include <set>

#define ENTT_USE_ATOMIC 1 //Enable thread-safe entt
#define ENTT_NO_ETO 1 //Enable empty components (tags) but reduce performance and increase memory consumption

#include <entt/entt.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <yaml-cpp/yaml.h>

#include <VGJS.h>

#include "eventhandling/eventhandling.hpp"

#include "Debugging/Profiler.h"
#include "Kernel/Logger.h"

#include "DataTypes/Pointers.h"
#include "DataTypes/Vector.h"
#include "DataTypes/Array.h"
#include "DataTypes/Map.h"
#include "DataTypes/Pair.h"
#include "DataTypes/Path.h"
#include "DataTypes/Queue.h"
#include "DataTypes/String.h"
#include "DataTypes/UnorderedMap.h"
#include "DataTypes/Utility.h"
#include "DataTypes/Delegates.h"

#include "Kernel/Utils.h"
#include "Kernel/Asserts.h"
#include "Kernel/UUID.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include "Math/Color.h"

#define LEV_PLATFORM_WINDOWS

#ifdef LEV_PLATFORM_WINDOWS

#define NOMINMAX
#include <Windows.h>
#include <windowsx.h>
#include <commdlg.h>

#endif
