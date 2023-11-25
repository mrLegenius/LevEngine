#pragma once
#include "Entity.h"

namespace LevEngine
{
	template <typename T>
	bool Entity::HasComponent() const
	{
		LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid");
		return m_Handle.any_of<T>();
	}

	template <typename T, typename ... Args>
	T& Entity::AddComponent(Args&&... args) const
	{
		LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid");
		LEV_CORE_ASSERT(!HasComponent<T>(), "Entity already has this component");

		T& component = m_Handle.emplace<T>(std::forward<Args>(args)...);
		return component;
	}

	template <typename T, typename ... Args>
	T& Entity::AddOrReplaceComponent(Args&&... args) const
	{
		T& component = m_Handle.emplace_or_replace<T>(std::forward<Args>(args)...);
		return component;
	}

	template <typename T>
	T& Entity::GetComponent() const
	{
		LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid");
		LEV_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component");

		return m_Handle.get<T>();
	}

	template <typename T>
	T& Entity::GetOrAddComponent() const
	{
		LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid");

		return m_Handle.get_or_emplace<T>();
	}

	template <typename T>
	void Entity::RemoveComponent() const
	{
		LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid");
		LEV_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component");

		auto _ = m_Handle.remove<T>();
	}

	template <typename T>
	Entity Entity::GetOtherEntity(const T& component)
	{
		auto& registry = *m_Handle.registry();
		const auto entity = entt::to_entity(registry, component);
		return Entity{ entt::handle{registry, entity} };
	}

	template <class TComponent>
	void Entity::RegisterMetaComponent()
	{
		using namespace entt::literals;
		entt::meta<TComponent>()
			.type(entt::type_hash<TComponent>::value())
			.template func<&add_component<TComponent>> ("add_component"_hs)
			.template func<&has_component<TComponent>>("has_component"_hs)
			.template func<&get_component<TComponent>> ("get_component"_hs)
			.template func<&remove_component<TComponent>> ("remove_component"_hs);
	}

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
}
