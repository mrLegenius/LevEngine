#include "pch.h"
#include "Scene.h"

#include "Physics/Physics.h"
#include "Renderer/Renderer.h"

#include "Assets.h"
#include "Entity.h"
#include "Components/Components.h"

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
    m_Registry.each([&](const entt::entity entityID)
    {
	    const auto entity = ConvertEntity(entityID);
	    callback(entity);
    });
}

Entity Scene::CreateEntity(const std::string& name)
{
    LEV_PROFILE_FUNCTION();

    return CreateEntity(UUID(), name);
}

Entity Scene::CreateEntity(UUID uuid, const std::string& name)
{
    LEV_PROFILE_FUNCTION();

    auto entity = Entity(entt::handle{ m_Registry, m_Registry.create() });
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<Transform>();
    entity.AddComponent<TagComponent>(name);

    return entity;
}

Entity Scene::ConvertEntity(const entt::entity entity)
{
    return Entity(entt::handle(m_Registry, entity));
}

void Scene::DestroyEntity(const Entity entity)
{
    LEV_PROFILE_FUNCTION();

    m_Registry.destroy(entity);
}

//template<typename... Component>
//static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
//{
//    /*int dummy[]{ 0, ([&]()
//        {
//            auto view = src.view<Component>();
//    for (auto srcEntity : view)
//    {
//        entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);
//
//        auto& srcComponent = src.get<Component>(srcEntity);
//        dst.emplace_or_replace<Component>(dstEntity, srcComponent);
//    }
//        }())... };*/
//}

//template<typename... Component>
//static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
//{
//    CopyComponent<Component...>(dst, src, enttMap);
//}

template<typename... Component>
static void CopyComponentIfExists(Entity dst, Entity src)
{
    /*([&]()
        {
            if (src.HasComponent<Component>())
            dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
        }(), ...);*/
}

template<typename... Component>
static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
{
    CopyComponentIfExists<Component...>(dst, src);
}

//Ref<Scene> Scene::Copy(Ref<Scene> other)
//{
//    Ref<Scene> newScene = CreateRef<Scene>();
//
//    newScene->m_ViewportWidth = other->m_ViewportWidth;
//    newScene->m_ViewportHeight = other->m_ViewportHeight;
//
//    auto& srcSceneRegistry = other->m_Registry;
//    auto& dstSceneRegistry = newScene->m_Registry;
//    std::unordered_map<UUID, entt::entity> enttMap;
//
//    // Create entities in new scene
//    auto idView = srcSceneRegistry.view<IDComponent>();
//    for (auto e : idView)
//    {
//        UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
//        const auto& name = srcSceneRegistry.get<TagComponent>(e).tag;
//        Entity newEntity = newScene->CreateEntity(uuid, name);
//        enttMap[uuid] = static_cast<entt::entity>(newEntity);
//    }
//
//    for(auto &&curr: registry.storage()) {
//        if(auto &storage = curr.second; storage.contains(src)) {
//            storage.push(dst, storage.value(src));
//        }
//    }
//    // Copy components (except IDComponent and TagComponent)
//    //CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);
//
//    return newScene;
//}


void Scene::DuplicateEntity(Entity entity)
{
    LEV_PROFILE_FUNCTION();

    Entity newEntity = CreateEntity(entity.GetName());
    CopyComponentIfExists(AllComponents{}, newEntity, entity);
}

Entity Scene::GetMainCameraEntity()
{
    LEV_PROFILE_FUNCTION();

    auto view = m_Registry.view<CameraComponent>();
    for (auto entity : view)
    {
        const auto& camera = view.get<CameraComponent>(entity);
        if (camera.isMain)
            return Entity(entt::handle(m_Registry, entity));
    }
    return { };
}
}