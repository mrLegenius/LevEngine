#pragma once
#include "entt/entt.hpp"
#include "Components/Components.h"
#include <sol/sol.hpp>

namespace LevEngine
{
class Scene;
class Entity
{
public:
	Entity() = default;
	Entity(const entt::handle handle) : m_Handle(handle) { }

	~Entity() = default;

	static void CreateLuaEntityBind(sol::state& lua, Scene* scene);

	template<class TComponent>
	static void RegisterMetaComponent();

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

	// TODO Remove
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
	Entity GetOtherEntity(const T& component);

private:
	entt::handle m_Handle;
};

// Lua functions

template <class TComponent>
auto add_component(Entity& entity, const sol::table& component, sol::this_state state);

template <class TComponent>
auto has_component(Entity& entity);

template <class TComponent>
auto get_component(Entity& entity, sol::this_state state);

template <class TComponent>
auto remove_component(Entity& entity);
}

#include "Entity.inl"