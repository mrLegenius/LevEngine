#pragma once

#include "Scene/System.h"
#include <eastl/vector.h>
#include <sol/sol.hpp>

namespace LevEngine::Scripting
{
	class ScriptingInitSystem : public System
	{
	public:
		// Inherited via System
		virtual void Update(float deltaTime, entt::registry& registry) override;

	};

	class ScriptingUpdateSystem: public System
	{
	public:
		// Inherited via System
		virtual void Update(float deltaTime, entt::registry& registry) override;
		
	};

	class ScriptingLateUpdateSystem : public System
	{
	public:
		// Inherited via System
		virtual void Update(float deltaTime, entt::registry& registry) override;

	};
}

