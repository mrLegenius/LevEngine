#pragma once

#include "Kernel/Core.h"

#include "Scene/System.h"
#include <eastl/vector.h>
#include <sol/sol.hpp>

namespace LevEngine::Scripting
{
	class LEV_API ScriptingInitSystem : public System
	{
	public:
		// Inherited via System
		virtual void Update(float deltaTime, entt::registry& registry) override;

	};

	class LEV_API ScriptingUpdateSystem: public System
	{
	public:
		// Inherited via System
		virtual void Update(float deltaTime, entt::registry& registry) override;
		
	};

	class LEV_API ScriptingLateUpdateSystem : public System
	{
	public:
		// Inherited via System
		virtual void Update(float deltaTime, entt::registry& registry) override;

	};

	class LEV_API ScriptingGUIRenderSystem : public System
	{
	public:
		// Inherited via System
		virtual void Update(float deltaTime, entt::registry& registry) override;

	};
}

