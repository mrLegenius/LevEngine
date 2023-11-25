#pragma once
#include "ScriptingManager.h"
#include "entt/entt.hpp"

namespace LevEngine::Scripting {

	template <class TComponent>
	entt::runtime_view& add_component_to_view(entt::registry* registry, entt::runtime_view& view)
	{
		return view.iterate(registry->storage<TComponent>());
	}


	template <class TComponent>
	entt::runtime_view& exclude_component_from_view(entt::registry* registry, entt::runtime_view& view)
	{
		return view.exclude(registry->storage<TComponent>());
	}

	template <class TComponent>
	void ScriptingManager::RegisterMetaComponent()
	{
		using namespace entt::literals;

		entt::meta<TComponent>()
			.type(entt::type_hash<TComponent>::value())
			.template func<&add_component_to_view<TComponent>>("add_component_to_view"_hs)
			.template func<&exclude_component_from_view<TComponent>>("exclude_component_from_view"_hs);
	}
}
