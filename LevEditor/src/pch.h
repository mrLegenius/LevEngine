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

#define ENTT_USE_ATOMIC 1 //Enable thread-safe entt
#define ENTT_NO_ETO 1 //Enable empty components (tags) but reduce performance and increase memory consumption

#include <entt/entt.hpp>

#include "eventhandling/eventhandling.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <yaml-cpp/yaml.h>

#include "LevEngine.h"

