#pragma once
#include "entt/entt.hpp"
#include "Components/Components.h"

namespace LevEngine
{
class Scene;
class Entity
{
public:
	Entity() = default;
	Entity(const entt::handle handle) : m_Handle(handle) { }

	~Entity() = default;

	template<typename T>
	bool HasComponent() const;

	template<typename T, typename ... Args>
	T& AddComponent(Args&& ... args) const;

	template<typename T, typename... Args>
	T& AddOrReplaceComponent(Args&&... args) const;

	template<typename T>
	T& GetComponent() const;

	template<typename T>
	T& GetOrAddComponent() const;

	template<typename T>
	void RemoveComponent() const;

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
	Entity GetOtherEntity(const T& component);

private:
	entt::handle m_Handle;
};

}

#include "Entity.inl"