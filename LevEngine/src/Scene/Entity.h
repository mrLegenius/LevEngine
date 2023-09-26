#pragma once
#include "entt/entt.hpp"
#include "Components/Components.h"

namespace LevEngine
{
class Entity
{
public:
	Entity() = default;
	Entity(const entt::handle handle) : m_Handle(handle) { }

	~Entity() = default;

	bool IsValid() const
	{
		return m_Handle && m_Handle.valid();
	}

	template<typename T>
	bool HasComponent() const
	{
		LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid");
		return m_Handle.any_of<T>();
	}

	template<typename T, typename ... Args>
	T& AddComponent(Args&& ... args) const
	{
		LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid");
		LEV_CORE_ASSERT(!HasComponent<T>(), "Entity already has this component");

		T& component = m_Handle.emplace<T>(std::forward<Args>(args)...);
		return component;
	}

	template<typename T, typename... Args>
	T& AddOrReplaceComponent(Args&&... args) const
	{
		T& component = m_Handle.emplace_or_replace<T>(std::forward<Args>(args)...);
		return component;
	}

	template<typename T>
	T& GetComponent() const
	{
		LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid");
		LEV_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component");

		return m_Handle.get<T>();
	}

	template<typename T>
	T& GetOrAddComponent() const
	{
		LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid");

		return m_Handle.get_or_emplace<T>();
	}

	template<typename T>
	void RemoveComponent() const
	{
		LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid");
		LEV_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component");

		auto _ = m_Handle.remove<T>();
	}

	template <typename T>
	void AddScript();

	UUID GetUUID() const { return GetComponent<IDComponent>().ID; }
	const String& GetName() const { return GetComponent<TagComponent>().tag; }

	operator bool() const { return m_Handle.entity() != entt::null && m_Handle.valid(); }
	operator uint32_t() const { return static_cast<uint32_t>(m_Handle.entity()); }
	operator entt::entity() const { return m_Handle.entity(); }

	bool operator ==(const Entity& other) const
	{
		return m_Handle.registry() == other.m_Handle.registry() && m_Handle.entity() == other.m_Handle.entity();
	}
	bool operator !=(const Entity& other) const
	{
		return !(*this == other);
	}

	template<typename  T>
	Entity GetOtherEntity(const T& component)
	{
		auto& registry = *m_Handle.registry();
		const auto entity = entt::to_entity(registry, component);
		return Entity{ entt::handle{registry, entity} };
	}

private:
	entt::handle m_Handle;
};
}