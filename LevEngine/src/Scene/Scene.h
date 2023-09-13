#pragma once
#include <entt/entt.hpp>

#include "System.h"
#include "Components/Transform/Transform.h"
#include "DataTypes/Pointers.h"
#include "Kernel/UUID.h"
#include "Physics/Physics.h"
#include "Renderer/Camera/SceneCamera.h"
#include "Systems/EventSystem.h"
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

	void ForEachEntity(const std::function<void(Entity)>& callback);

	Entity CreateEntity(const String& name = "Entity");
	Entity CreateEntity(UUID uuid, const String& name);

	void DestroyEntity(entt::entity entity);
	void DestroyEntity(Entity entity);
	static void GetAllChildren(Entity entity, Vector<Entity>& entities);

	Entity DuplicateEntity(Entity entity);
	Entity DuplicateEntity(Entity entity, Entity parent);

	void OnCameraComponentAdded(entt::registry& registry, entt::entity entity);

	void RegisterUpdateSystem(Ref<System> system)
	{
		m_UpdateSystems.emplace_back(system);
	}

	void RegisterLateUpdateSystem(Ref<System> system)
	{
		m_LateUpdateSystems.emplace_back(system);
	}

	template<typename T>
	void RegisterOneFrame()
	{
		m_EventSystems.emplace_back(new EventSystem<T>);
	}

	Entity GetEntityBy(Transform* value);

private:
	entt::registry m_Registry;
	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	friend class Entity;

	Entity ConvertEntity(entt::entity entity);

	Vector<Ref<System>> m_UpdateSystems;
	Vector<Ref<System>> m_LateUpdateSystems;
	Vector<Ref<System>> m_EventSystems;
};
}
