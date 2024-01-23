#pragma once

namespace LevEngine
{
	template <typename T>
	bool Entity::HasComponent() const
	{
		std::lock_guard lock(s_Mutex);

		LEV_CORE_ASSERT(m_Handle, "Entity is not valid");

		return m_Handle.any_of<T>();
	}

	template <typename T, typename ... Args>
	T& Entity::AddComponent(Args&&... args) const
	{
		std::lock_guard lock(s_Mutex);

		LEV_CORE_ASSERT(m_Handle, "Entity is not valid");
		LEV_CORE_ASSERT(!HasComponent<T>(), "Entity already has this component");

		T& component = m_Handle.emplace<T>(std::forward<Args>(args)...);
		return component;
	}

	template <typename T, typename ... Args>
	T& Entity::AddOrReplaceComponent(Args&&... args) const
	{
		std::lock_guard lock(s_Mutex);

		LEV_CORE_ASSERT(m_Handle, "Entity is not valid");

		T& component = m_Handle.emplace_or_replace<T>(std::forward<Args>(args)...);
		return component;
	}

	template <typename T>
	T& Entity::GetComponent() const
	{
		std::lock_guard lock(s_Mutex);

		LEV_CORE_ASSERT(m_Handle, "Entity is not valid");
		LEV_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component");

		return m_Handle.get<T>();
	}

	template <typename T>
	T& Entity::GetOrAddComponent() const
	{
		std::lock_guard lock(s_Mutex);

		LEV_CORE_ASSERT(m_Handle, "Entity is not valid");

		return m_Handle.get_or_emplace<T>();
	}

	template <typename T>
	void Entity::RemoveComponent() const
	{
		std::lock_guard lock(s_Mutex);

		LEV_CORE_ASSERT(m_Handle, "Entity is not valid");
		LEV_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component");

		auto _ = m_Handle.remove<T>();
	}

	template <typename T>
	Entity Entity::GetOtherEntity(const T& component)
	{
		std::lock_guard lock(s_Mutex);

		LEV_CORE_ASSERT(m_Handle, "Entity is not valid");

		auto& registry = *m_Handle.registry();
		const auto entity = entt::to_entity(registry, component);
		return Entity{ entt::handle{registry, entity} };
	}
}
