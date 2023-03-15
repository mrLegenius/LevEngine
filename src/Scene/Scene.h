#pragma once
#include <GameObject.h>
#include <entt/entt.hpp>

#include "Kernel/PointerUtils.h"
#include "Renderer/Camera/SceneCamera.h"

class Entity;
class Scene
{
public:
	Scene() = default;
	~Scene();

	void OnUpdateRuntime(float deltaTime);
	void Render();
	void OnRenderRuntime();
	void DirectionalLightSystem();
	void PointLightsSystem();

	void OnViewportResized(uint32_t width, uint32_t height);

	Entity CreateEntity(const std::string& name = "Entity");
	Entity CreateEntity(UUID uuid, const std::string& name);

	void DestroyEntity(Entity entity);

	//static Ref<Scene> Copy(Ref<Scene> other);
	void DuplicateEntity(Entity entity);

	template<typename T>
	void OnComponentAdded(Entity entity, T& component);
	Entity GetMainCameraEntity();
private:
	entt::registry m_Registry;
	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	friend class Entity;

	void MeshRenderSystem();
	void OrbitCameraSystem(float deltaTime);

	void SkyboxRenderSystem();
};