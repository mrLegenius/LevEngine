#pragma once
#include <sol/sol.hpp>

namespace LevEngine::Scripting {
	class MathLuaBindings {
	public:
		static void CreateLuaBindings(sol::state& lua);
	};
}