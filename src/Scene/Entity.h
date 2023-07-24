#pragma once
#include "entt/entt.hpp"
#include "Components/Components.h"

namespace LevEngine
{
class Entity
{
public:
	Entity() = default;
	Entity(entt::handle handle) : m_Handle(handle) { }

	~Entity() = default;

	template<typename T>
	bool HasComponent()
	{
		assert(m_Handle.valid() && "Entity is not valid!");
		return m_Handle.any_of<T>();
	}

	template<typename T, typename ... Args>
	T& AddComponent(Args&& ... args)
	{
		assert(m_Handle.valid() && "Entity is not valid!");
		assert(!HasComponent<T>() && "Entity already has this component!");

		T& component = m_Handle.emplace<T>(std::forward<Args>(args)...);
		return component;
	}

	template<typename T, typename... Args>
	T& AddOrReplaceComponent(Args&&... args)
	{
		T& component = m_Handle.emplace_or_replace<T>(std::forward<Args>(args)...);
		return component;
	}

	template<typename T>
	T& GetComponent()
	{
		assert(m_Handle.valid() && "Entity is not valid!");
		assert(HasComponent<T>() && "Entity does not have this component!");

		return m_Handle.get<T>();
	}

	template<typename T>
	void RemoveComponent()
	{
		assert(m_Handle.valid() && "Entity is not valid!");
		assert(HasComponent<T>() && "Entity does not have this component!");

		m_Handle.remove<T>();
	}

	template <typename T>
	void AddScript();

	UUID GetUUID() { return GetComponent<IDComponent>().ID; }
	const std::string& GetName() { return GetComponent<TagComponent>().tag; }

	operator bool() const { return m_Handle.entity() != entt::null; }
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
private:
	entt::handle m_Handle;
};
}