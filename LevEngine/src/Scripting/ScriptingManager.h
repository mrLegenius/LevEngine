#pragma once

#include <sol/sol.hpp>

namespace LevEngine {
	class Scene;

	namespace Scripting
	{	

		class ScriptingManager
		{
		public:
			ScriptingManager();
			void Init();
			bool LoadScripts();

			void RegisterSystems(Scene* scene);

			template <class TComponent>
			void RegisterMetaComponent();

			void CreateRegistryBind(entt::registry& registry) const;

			void Shutdown();

			Ref<sol::state> GetLuaState();

		private:
			UnorderedMap<String, sol::table> m_Systems;
			UnorderedMap<String, sol::table> m_Components;

			Ref<sol::state> m_Lua;
		};

		template <class TComponent>
		entt::runtime_view& add_component_to_view(entt::registry* registry, entt::runtime_view& view);

		template <class TComponent>
		entt::runtime_view& exclude_component_from_view(entt::registry* registry, entt::runtime_view& view);
	}
}

#include "ScriptingManager.inl"

