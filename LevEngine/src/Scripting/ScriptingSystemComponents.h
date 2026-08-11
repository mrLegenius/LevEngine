#pragma once

#include "Kernel/Core.h"
#include <eastl/vector.h>
#include <sol/sol.hpp>

namespace LevEngine::Scripting
{
	struct LEV_API ScriptingInitComponent
	{
		sol::protected_function init{ sol::lua_nil };
	};

	struct LEV_API ScriptingUpdateComponent
	{
		sol::protected_function update{ sol::lua_nil };
	};

	struct LEV_API ScriptingLateUpdateComponent
	{
		sol::protected_function lateUpdate{ sol::lua_nil };
	};

	struct LEV_API ScriptingGUIRenderComponent
	{
		sol::protected_function GUIRender{ sol::lua_nil };
	};
}
