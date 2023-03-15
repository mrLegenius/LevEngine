#include "Scene.h"

#include <GameObject.h>

#include "Assets.h"
#include "Components/Components.h"
#include "Entity.h"
#include "OrbitCamera.h"
#include "Components/SkyboxRenderer.h"
#include "Kernel/Application.h"
#include "Renderer/Renderer3D.h"

std::shared_ptr<D3D11ConstantBuffer> m_DirLightConstantBuffer;

using namespace DirectX::SimpleMath;

Scene::~Scene()
{
    m_Registry.clear();
}

void Scene::OnUpdateRuntime(const float deltaTime)
{
    OrbitCameraSystem(deltaTime);
}

void Scene::Render()
{
    SceneCamera* mainCamera = nullptr;
    Matrix viewMatrix;
    Vector3 cameraPosition;

    {
        auto view = m_Registry.view<Transform>();
        for (auto entity : view)
        {
            auto& transform = view.get<Transform>(entity);
            transform.RecalculateModel();
        }
    }

    //Render Scene
    {
        const auto group = m_Registry.view<Transform, CameraComponent>();
        for (auto entity : group)
        {
            auto [transform, camera] = group.get<Transform, CameraComponent>(entity);

            if (camera.isMain)
            {
                mainCamera = &camera.camera;
                viewMatrix = transform.GetModel().Invert();
                cameraPosition = transform.GetWorldPosition();
                break;
            }
        }
    }

    if (!mainCamera) return;

    Renderer3D::BeginScene(*mainCamera, viewMatrix, cameraPosition);

    //m_DirLightConstantBuffer = std::make_shared<D3D11ConstantBuffer>(sizeof LightningData, 2);

    //auto& window = Application::Get().GetWindow();
    //mainCamera->SetViewportSize(window.GetWidth(), window.GetHeight());

    //const LightningData lightningData
    //{
    //    DirLightData{},
    //    {},
    //    0,
    //};
    //m_DirLightConstantBuffer->SetData(&lightningData, sizeof LightningData);

    MeshRenderSystem();

    SkyboxRenderSystem();

    Renderer3D::EndScene();
}

void Scene::OnRenderRuntime()
{
    SceneCamera* mainCamera = nullptr;
    Matrix cameraTransform;
    Vector3 cameraPosition;

    //Reset
    {
        auto view = m_Registry.view<Transform>();
        for (auto entity : view)
        {
            auto& transform = view.get<Transform>(entity);
            transform.RecalculateModel();
        }
    }

    //Render Scene
    {
	    const auto group = m_Registry.view<Transform, CameraComponent>();
        for (auto entity : group)
        {
            auto [transform, camera] = group.get<Transform, CameraComponent>(entity);

            if (camera.isMain)
            {
                mainCamera = &camera.camera;
                cameraTransform = transform.GetModel().Invert();
                cameraPosition = transform.GetWorldPosition();
                break;
            }
        }
    }

    if (mainCamera)
    {
        Renderer3D::BeginScene(*mainCamera, cameraTransform, cameraPosition);

        DirectionalLightSystem();
        PointLightsSystem();
        Renderer3D::UpdateLights();

        MeshRenderSystem();

        SkyboxRenderSystem();

        Renderer3D::EndScene();
    }
}

void Scene::DirectionalLightSystem()
{
    auto view = m_Registry.view<Transform, DirectionalLightComponent>();
    for (auto entity : view)
    {
        auto [transform, light] = view.get<Transform, DirectionalLightComponent>(entity);

        Renderer3D::SetDirLight(transform.GetWorldRotation(), light);
    }
}

void Scene::PointLightsSystem()
{
    auto view = m_Registry.view<Transform, PointLightComponent>();
    for (auto entity : view)
    {
        auto [transform, light] = view.get<Transform, PointLightComponent>(entity);

        Renderer3D::AddPointLights(transform.GetWorldPosition(), light);
    }
}

void Scene::SkyboxRenderSystem()
{
    auto view = m_Registry.view<Transform, SkyboxRendererComponent>();
    for (auto entity : view)
    {
    	auto [transform, skybox] = view.get<Transform, SkyboxRendererComponent>(entity);
        Renderer3D::DrawSkybox(skybox);
        break;
    }
}

void Scene::MeshRenderSystem()
{
    auto view = m_Registry.view<Transform, MeshRendererComponent>();
    for (auto entity : view)
    {
        auto [transform, mesh] = view.get<Transform, MeshRendererComponent>(entity);
        Renderer3D::DrawMesh(transform.GetModel(), mesh);
    }
}

void Scene::OrbitCameraSystem(const float deltaTime)
{
    auto view = m_Registry.view<Transform, OrbitCamera>();
    for (const auto entity : view)
    {
        auto [transform, camera] = view.get<Transform, OrbitCamera>(entity);

        camera.Update(deltaTime);

		transform.SetWorldPosition(camera.position);
		transform.SetWorldRotation(camera.rotation);
    }
}


void Scene::OnViewportResized(const uint32_t width, const uint32_t height)
{
    m_ViewportWidth = width;
    m_ViewportHeight = height;

    auto view = m_Registry.view<CameraComponent>();
    for (auto entity : view)
    {
        auto& camera = view.get<CameraComponent>(entity);

        if (camera.fixedAspectRatio) continue;

        //camera.camera.SetViewportSize(width, height);
    }
}

Entity Scene::CreateEntity(const std::string& name)
{
    return CreateEntity(UUID(), name);
}

Entity Scene::CreateEntity(UUID uuid, const std::string& name)
{
    auto entity = Entity(m_Registry.create(), this);
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<Transform>();
    entity.AddComponent<TagComponent>(name);

    return entity;
}

void Scene::DestroyEntity(const Entity entity)
{
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
//    // Copy components (except IDComponent and TagComponent)
//    CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);
//
//    return newScene;
//}


void Scene::DuplicateEntity(Entity entity)
{
    Entity newEntity = CreateEntity(entity.GetName());
    CopyComponentIfExists(AllComponents{}, newEntity, entity);
}

Entity Scene::GetMainCameraEntity()
{
    auto view = m_Registry.view<CameraComponent>();
    for (auto entity : view)
    {
        const auto& camera = view.get<CameraComponent>(entity);
        if (camera.isMain)
            return { entity, this };
    }
    return { };
}

template <typename T>
void Scene::OnComponentAdded(Entity entity, T& component)
{

}

template <>
void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
{
}
template <>
void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
}
template <>
void Scene::OnComponentAdded<Transform>(Entity entity, Transform& component)
{
}
template <>
void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
{
    //component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
}
template <>
void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component)
{
}
template <>
void Scene::OnComponentAdded<SkyboxRendererComponent>(Entity entity, SkyboxRendererComponent& component)
{
    
}
template <>
void Scene::OnComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component)
{
}
template <>
void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component)
{
}
template <>
void Scene::OnComponentAdded<OrbitCamera>(Entity entity, OrbitCamera& component)
{
    //component.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
}

template <>
void Scene::OnComponentAdded<SkyboxRenderer>(Entity entity, SkyboxRenderer& component)
{
}