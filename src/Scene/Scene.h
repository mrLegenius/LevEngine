#pragma once
#include <entt/entt.hpp>

#include "System.h"
#include "Components/Components.h"
#include "Kernel/PointerUtils.h"
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
	void CollisionDetectionSystem();
	void UpdateCollisionList();
	void SphereCollisionSystem();
	void AABBSphereCollisionSystem();
	void AABBCollisionResolveSystem();
	static void RegisterCollision(entt::entity i, entt::entity j);
	void OnRender();
	void DirectionalLightSystem();
	void PointLightsSystem();

	void OnViewportResized(uint32_t width, uint32_t height);

	Entity CreateEntity(const std::string& name = "Entity");
	Entity CreateEntity(UUID uuid, const std::string& name);

	void DestroyEntity(Entity entity);

	//static Ref<Scene> Copy(Ref<Scene> other);
	void DuplicateEntity(Entity entity);

	void OnCameraComponentAdded(entt::registry& registry, entt::entity entity);
	Entity GetMainCameraEntity();

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
private:
	entt::registry m_Registry;
	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	friend class Entity;

	void MeshRenderSystem();
	void MeshShadowSystem();
	void MeshDeferredSystem();

	Entity ConvertEntity(entt::entity entity);

	std::vector<Ref<System>> m_UpdateSystems;
	std::vector<Ref<System>> m_LateUpdateSystems;
	std::vector<Ref<System>> m_EventSystems;
};
}
