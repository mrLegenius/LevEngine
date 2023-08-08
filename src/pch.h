#pragma once

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

#include "Debugging/Profiler.h"
#include "Kernel/Logger.h"
#include "Kernel/Asserts.h"
#include "Kernel/PointerUtils.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Renderer/Viewport.h"
#include "Renderer/Rect.h"

#ifdef LEV_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
