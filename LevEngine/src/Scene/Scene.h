#pragma once
#include <entt/entt.hpp>

#include "System.h"
#include "Components/Transform/Transform.h"
#include "DataTypes/Delegates.h"
#include "DataTypes/Pointers.h"
#include "Kernel/UUID.h"
#include "Physics/Physics.h"
#include "Renderer/Camera/SceneCamera.h"
#include "Systems/EventSystem.h"

#include "Physics/PhysX/PhysX.h"

namespace LevEngine
{
class Entity;
class Scene
{
public:
	Scene() = default;
	~Scene();

	void OnUpdate(float deltaTime);
	void OnPhysics(float deltaTime);
	void OnLateUpdate(float deltaTime);
	void OnRender();
	void OnRender(SceneCamera* mainCamera, const Transform* cameraTransform);

	void OnViewportResized(uint32_t width, uint32_t height);

	void ForEachEntity(const Action<Entity>& callback);

	Entity CreateEntity(const String& name = "Entity");
	Entity CreateEntity(UUID uuid, const String& name);

	void DestroyEntity(entt::entity entity);
	void DestroyEntity(Entity entity);
	static void GetAllChildren(Entity entity, Vector<Entity>& entities);

	Entity DuplicateEntity(Entity entity);
	Entity DuplicateEntity(Entity entity, Entity parent);

	void OnCameraComponentAdded(entt::registry& registry, entt::entity entity);

	template<typename T>
	void RegisterUpdateSystem()
	{
		static_assert(eastl::is_base_of_v<System, T>, "T must derive from System");
		m_UpdateSystems.emplace_back(CreateScope<T>());
	}

	template<typename T>
	void RegisterLateUpdateSystem()
	{
		static_assert(eastl::is_base_of_v<System, T>, "T must derive from System");
		m_LateUpdateSystems.emplace_back(CreateScope<T>());
	}

	template<typename T>
	void RegisterOneFrame()
	{
		m_EventSystems.emplace_back(CreateScope<EventSystem<T>>());
	}

	Entity GetEntityBy(Transform* value);

private:
	entt::registry m_Registry;
	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	friend class Entity;

	Entity ConvertEntity(entt::entity entity);

	Vector<Scope<System>> m_UpdateSystems;
	Vector<Scope<System>> m_LateUpdateSystems;
	Vector<Scope<System>> m_EventSystems;

	static inline bool bTestPhysX = true;
};
}
