#pragma once
#include <eastl/vector.h>
#include <sol/sol.hpp>

namespace LevEngine::Scripting
{
	struct ScriptingInitComponent
	{
		sol::protected_function init{ sol::lua_nil };
	};

	struct ScriptingUpdateComponent
	{
		sol::protected_function update{ sol::lua_nil };
	};

	struct ScriptingLateUpdateComponent
	{
		sol::protected_function lateUpdate{ sol::lua_nil };
	};
}
