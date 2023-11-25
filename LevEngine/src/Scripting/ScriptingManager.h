#pragma once

#include <DataTypes/Path.h>
#include <sol/sol.hpp>
#include <entt/entt.hpp>
#include "DataTypes/Pointers.h"
#include "DataTypes/UnorderedMap.h"
#include "ScriptingComponent.h"

namespace LevEngine {
	class Scene;

	namespace Scripting {

		

		class ScriptingManager
		{
		public:
			static void Init(sol::state& lua);
			static bool LoadScripts(sol::state& lua, const Path& projectPath);

			static void RegisterSystems(Scene* const scene);

			template <class TComponent>
			static void RegisterMetaComponent();

			static void CreateRegistryBind(sol::state& lua, entt::registry& registry);

			static void Shutdown();

		private:
			inline static const String ScriptsDatabaseFilename = "ScriptsDatabase.asset";
			inline static const String ScriptsDatabaseDir = "scripts";

			inline static UnorderedMap<String, sol::table> m_Systems;
		};

		template <class TComponent>
		entt::runtime_view& add_component_to_view(entt::registry* registry, entt::runtime_view& view);

		template <class TComponent>
		entt::runtime_view& exclude_component_from_view(entt::registry* registry, entt::runtime_view& view);
	}
}

#include "ScriptingManager.inl"

