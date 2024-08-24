#pragma once
#include "Entity.h"
#include "Systems/EventSystem.h"

namespace LevEngine 
{
	template<typename T>
	void Scene::RegisterComponentOnConstruct()
	{
		m_Registry.on_construct<T>().connect<&Scene::OnComponentConstruct<T>>(this);
	}

	template<typename T>
	void Scene::RegisterComponentOnDestroy()
	{
		m_Registry.on_destroy<T>().connect<&Scene::OnComponentDestroy<T>>(this);
	}

	template<typename TComponent>
	void Scene::OnComponentConstruct(entt::registry& registry, const entt::entity entity)
	{
		if constexpr ( requires { TComponent::OnConstruct(Entity{}); })
			TComponent::OnConstruct(Entity(entt::handle(registry, entity)));
	}

	template<typename TComponent>
	void Scene::OnComponentDestroy(entt::registry& registry, const entt::entity entity)
	{
		if constexpr ( requires { TComponent::OnDestroy(Entity{}); })
			TComponent::OnDestroy(Entity(entt::handle(registry, entity)));
	}

	template <class T>
	void Scene::RegisterInitSystem()
	{
		static_assert(eastl::is_base_of_v<System, T>, "T must derive from System");
		m_InitSystems.emplace_back(CreateScope<T>());
	}

	template<typename T>
	void Scene::RegisterUpdateSystem()
	{
		static_assert(eastl::is_base_of_v<System, T>, "T must derive from System");
		m_UpdateSystems.emplace_back(CreateScope<T>());
	}

	template<typename T>
	void Scene::RegisterLateUpdateSystem()
	{
		static_assert(eastl::is_base_of_v<System, T>, "T must derive from System");
		m_LateUpdateSystems.emplace_back(CreateScope<T>());
	}
	template<typename T>
	void Scene::RegisterOneFrame()
	{
		m_EventSystems.emplace_back(CreateScope<EventSystem<T>>());
	}

	template <typename T>
	void Scene::RegisterGUIRenderSystem()
	{
		static_assert(eastl::is_base_of_v<System, T>, "T must derive from System");
		m_GUIRenderSystems.emplace_back(CreateScope<T>());
	}

	template<typename T>
	Entity Scene::GetEntityByComponent(T* value)
	{
		const auto entity = entt::to_entity(m_Registry, *value);
		return ConvertEntity(entity);
	}
}
