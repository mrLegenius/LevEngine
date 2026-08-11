#pragma once

#include "Kernel/Core.h"
#include <sol/sol.hpp>

namespace LevEngine::Scripting {
	class LEV_API MathLuaBindings {
	public:
		static void CreateLuaBindings(sol::state& lua);
	};
}