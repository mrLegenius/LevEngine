#include "levpch.h"
#include "Scene.h"

#include "Physics/Physics.h"
#include "Renderer/Renderer.h"

#include "Entity.h"
#include "Components/Components.h"
#include "Components/Camera/Camera.h"

namespace LevEngine
{
Scene::~Scene()
{
    LEV_PROFILE_FUNCTION();

    m_Registry.clear();
}

void Scene::OnUpdate(const float deltaTime)
{
    LEV_PROFILE_FUNCTION();

	for (const auto system : m_UpdateSystems)
		system->Update(deltaTime, m_Registry);
}

void Scene::OnPhysics(const float deltaTime)
{
    Physics::Process(m_Registry, deltaTime);
}

void Scene::OnRender()
{
    Renderer::Render(m_Registry);
}

void Scene::OnRender(SceneCamera* mainCamera, const Transform* cameraTransform)
{
    Renderer::Render(m_Registry, mainCamera, cameraTransform);
}

void Scene::OnLateUpdate(const float deltaTime)
{
    LEV_PROFILE_FUNCTION();

    for (const auto system : m_LateUpdateSystems)
        system->Update(deltaTime, m_Registry);

    for (const auto system : m_EventSystems)
        system->Update(deltaTime, m_Registry);
}

void Scene::OnViewportResized(const uint32_t width, const uint32_t height)
{
    LEV_PROFILE_FUNCTION();

    m_ViewportWidth = width;
    m_ViewportHeight = height;

    const auto view = m_Registry.view<CameraComponent>();
    for (const auto entity : view)
    {
        auto& camera = view.get<CameraComponent>(entity);

        if (camera.fixedAspectRatio) continue;

        camera.camera.SetViewportSize(width, height);
    }
}

void Scene::ForEachEntity(const std::function<void(Entity)>& callback)
{
	for (const auto entityId : m_Registry.storage<entt::entity>())
	{
        if (!m_Registry.valid(entityId)) continue;

        const auto entity = ConvertEntity(entityId);
        callback(entity);
	}
}

Entity Scene::CreateEntity(const String& name)
{
    LEV_PROFILE_FUNCTION();

    return CreateEntity(UUID(), name);
}

Entity Scene::CreateEntity(UUID uuid, const String& name)
{
    LEV_PROFILE_FUNCTION();

    auto entity = Entity(entt::handle{ m_Registry, m_Registry.create() });
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<Transform>(entity);
    entity.AddComponent<TagComponent>(name);

    return entity;
}

Entity Scene::ConvertEntity(const entt::entity entity)
{
    return Entity(entt::handle(m_Registry, entity));
}

void Scene::DestroyEntity(const entt::entity entity)
{
    DestroyEntity(ConvertEntity(entity));
}
    
void Scene::DestroyEntity(const Entity entity)
{
    LEV_PROFILE_FUNCTION();

    Vector<Entity> entitiesToDestroy;

    auto& parentTransform = entity.GetComponent<Transform>();
    parentTransform.SetParent(Entity{});

    GetAllChildren(entity, entitiesToDestroy);

    m_Registry.destroy(entitiesToDestroy.begin(), entitiesToDestroy.end());
}

void Scene::GetAllChildren(Entity entity, Vector<Entity>& entities)
{
	const auto& parentTransform = entity.GetComponent<Transform>();

	for (const auto child : parentTransform.GetChildren())
		GetAllChildren(child, entities);

	entities.emplace(entities.begin(), entity);
}

Entity Scene::DuplicateEntity(const Entity entity)
{
    return DuplicateEntity(entity, entity.GetComponent<Transform>().GetParent());
}
    
Entity Scene::DuplicateEntity(const Entity entity, const Entity parent)
{
    LEV_PROFILE_FUNCTION();
    
    const entt::entity newEntity = m_Registry.create();

    //Copy component by component
    for(auto &&curr: m_Registry.storage())
        if(auto &storage = curr.second; storage.contains(entity))
            storage.push(newEntity, storage.value(entity));

    //Assign new UUID
    m_Registry.replace<IDComponent>(newEntity, UUID());

    //Restore transform
    auto duplicatedEntity = ConvertEntity(newEntity);
    Transform& transform = m_Registry.replace<Transform>(newEntity, duplicatedEntity);
    const Transform& oldTransform = entity.GetComponent<Transform>();
    transform.SetWorldPosition(oldTransform.GetWorldPosition());
    transform.SetWorldRotation(oldTransform.GetWorldRotation());
    transform.SetWorldScale(oldTransform.GetWorldScale());
    transform.SetParent(parent);

    //Copy all children
    for (const auto child : oldTransform.GetChildren())
        DuplicateEntity(child, duplicatedEntity);

    return duplicatedEntity;
}

Entity Scene::GetEntityBy(Transform* value)
{
	const auto entity = entt::to_entity(m_Registry, *value);
    return ConvertEntity(entity);
}
}
