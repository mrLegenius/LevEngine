#pragma once
#include "LuaComponentsBinder.h"
#include "Scene/Entity.h"

namespace LevEngine::Scripting
{
	template <class TComponent>
	auto add_component(Entity& entity, const sol::table& component, sol::this_state state)
	{
		auto& addedComponent = entity.AddComponent<TComponent>(
			component.valid() ? component.as<TComponent>() : TComponent{}
		);

		return sol::make_reference(state, std::ref(addedComponent));
	}

	template <class TComponent>
	auto has_component(Entity& entity)
	{
		return entity.HasComponent<TComponent>();
	}

	template <class TComponent>
	auto get_component(Entity& entity, sol::this_state state)
	{
		auto& component = entity.GetComponent<TComponent>();

		return sol::make_reference(state, std::ref(component));
	}

	template <class TComponent>
	auto remove_component(Entity& entity)
	{
		entity.RemoveComponent<TComponent>();
	}

	template <class TComponent>
	auto get_or_add_component(Entity& entity, sol::this_state state)
	{
		auto& component = entity.GetOrAddComponent<TComponent>();

		return sol::make_reference(state, std::ref(component));
	}

	template <class TComponent>
	void LuaComponentsBinder::RegisterMetaComponent()
	{
		using namespace entt::literals;
		entt::meta<TComponent>()
			.type(entt::type_hash<TComponent>::value())
			.template func<&add_component<TComponent>>("add_component"_hs)
			.template func<&has_component<TComponent>>("has_component"_hs)
			.template func<&get_component<TComponent>>("get_component"_hs)
			.template func<&remove_component<TComponent>>("remove_component"_hs)
			.template func<&get_or_add_component<TComponent>>("get_or_add_component"_hs);
	}
}
