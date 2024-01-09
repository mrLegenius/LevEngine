#pragma once
#include "Entity.h"
#include "Scene.h"
#include "Systems/EventSystem.h"

namespace LevEngine 
{
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
