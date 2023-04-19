#include "Scene.h"

#include <Physics/Physics.h>

#include "Assets.h"
#include "Components/Components.h"
#include "Entity.h"
#include "OrbitCamera.h"
#include "Components/SkyboxRenderer.h"
#include "Debugging/Profiler.h"
#include "Kernel/Application.h"
#include "Physics/Components/Rigidbody.h"
#include "Physics/Systems/VelocityUpdateSystem.h"
#include "Physics/Events/CollisionBeginEvent.h"
#include "Physics/Events/CollisionEndEvent.h"
#include "Physics/Components/CollisionEvent.h"
#include "Physics/Systems/ForcesClearSystem.h"
#include "Physics/Systems/PositionUpdateSystem.h"
#include "Renderer/ParticlePass.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/RenderParams.h"
#include "Renderer/ShadowMapPass.h"
#include "Renderer/SkyboxPass.h"

struct CollisionInfo;
using namespace DirectX::SimpleMath;

Scene::~Scene()
{
    LEV_PROFILE_FUNCTION();

    m_Registry.clear();
}

void Scene::OnUpdate(const float deltaTime)
{
    LEV_PROFILE_FUNCTION();

	for (const auto system : m_UpdateSystems)
	{
        system->Update(deltaTime, m_Registry);
	}
}

static float dTOffset = 0;

void Scene::OnPhysics(const float deltaTime)
{
    LEV_PROFILE_FUNCTION();

    constexpr float iterationDt = 1.0f / 120.0f; //Ideally we'll have 120 physics updates a second 
    dTOffset += deltaTime; //We accumulate time delta here - there might be remainders from previous frame!

    const int iterationCount = static_cast<int>(dTOffset / iterationDt); //And split it up here

    const float subDt = deltaTime / static_cast<float>(iterationCount);	//How many seconds per iteration do we get?

    VelocityUpdateSystem(deltaTime, m_Registry);

    for (int i = 0; i < iterationCount; ++i)
    {
        /*if (useBroadPhase) { //TODO: Add optimization
            BroadPhase();
            NarrowPhase();
        }
        else {*/
        CollisionDetectionSystem();
        //}

        //This is our simple iterative solver - 
        //we just run things multiple times, slowly moving things forward
        //and then rechecking that the constraints have been met

        constexpr int constraintIterationCount = 10;
        const float constraintDt = subDt / static_cast<float>(constraintIterationCount);

        for (int j = 0; j < constraintIterationCount; ++j) {
            //UpdateConstraints(constraintDt);
            UpdatePositionSystem(constraintDt, m_Registry);
        }

        dTOffset -= iterationDt;
    }
    ForcesClearSystem(deltaTime, m_Registry);//Once we've finished with the forces, reset them to zero

    UpdateCollisionList(); //Remove any old collisions
}

void Scene::OnLateUpdate(const float deltaTime)
{
    LEV_PROFILE_FUNCTION();

    for (const auto system : m_LateUpdateSystems)
        system->Update(deltaTime, m_Registry);

    for (const auto system : m_EventSystems)
        system->Update(deltaTime, m_Registry);
}

void Scene::CollisionDetectionSystem()
{
    LEV_PROFILE_FUNCTION();

    AABBCollisionResolveSystem();
    SphereCollisionSystem();
    AABBSphereCollisionSystem();
}

using entity_pair = std::pair<entt::entity, entt::entity>;
std::map<entity_pair, int> collisions;

void Scene::RegisterCollision(entt::entity i, entt::entity j)
{
    auto exist = collisions.find(entity_pair(i, j));
    if (exist == collisions.end())
        exist = collisions.find(entity_pair(j, i));

    if (exist == collisions.end())
        collisions.emplace(entity_pair(i, j), NumCollisionFrames);
    else
        exist->second = NumCollisionFrames - 1;
}

void Scene::UpdateCollisionList()
{
    LEV_PROFILE_FUNCTION();

    std::vector<entity_pair> pairsToDelete;

    for (auto [pair, frames] : collisions)
    {
        auto a = pair.first;
        auto b = pair.second;
        if (frames == NumCollisionFrames)
        {
	        if (const CollisionEvents* eventA = m_Registry.try_get<CollisionEvents>(a))
	        {
                if (eventA->onCollisionBegin)
                    eventA->onCollisionBegin(ConvertEntity(a), ConvertEntity(b));
	        }

            if (const CollisionEvents* eventB = m_Registry.try_get<CollisionEvents>(b))
            {
                if (eventB->onCollisionBegin)
                    eventB->onCollisionBegin(ConvertEntity(b), ConvertEntity(a));
            }
        }

        collisions[pair]--;

        if (frames < 0)
        {
            if (const CollisionEvents* eventA = m_Registry.try_get<CollisionEvents>(a))
            {
                if (eventA->onCollisionEnd)
                    eventA->onCollisionEnd(ConvertEntity(a), ConvertEntity(b));
            }


            if (const CollisionEvents* eventB = m_Registry.try_get<CollisionEvents>(b))
            {
                if (eventB->onCollisionEnd)
                    eventB->onCollisionEnd(ConvertEntity(b), ConvertEntity(a));
            }

            pairsToDelete.emplace_back(pair);
        }
    }

    for (auto pair : pairsToDelete)
	    collisions.erase(pair);
}

void Scene::SphereCollisionSystem()
{
    LEV_PROFILE_FUNCTION();

    const auto view = m_Registry.group<>(entt::get<Transform, Rigidbody, SphereCollider>);
    const auto first = view.begin();
    const auto last = view.end();

    for (auto i = first; i != last; ++i)
    {
        auto [transform1, rigidbody1, colliderA] = view.get<Transform, Rigidbody, SphereCollider>(*i);
        for (auto j = i+1; j != last; ++j)
        {
            auto [transform2, rigidbody2, colliderB] = view.get<Transform, Rigidbody, SphereCollider>(*j);

            if (!rigidbody1.enabled || !rigidbody2.enabled) continue;
            CollisionInfo info;

            if (Physics::HasSphereIntersection(
                colliderA,
                transform1,
                colliderB,
                transform2,
                info))
            {
                Physics::HandleCollision(transform1, rigidbody1, transform2, rigidbody2, info.point);
                RegisterCollision(*i, *j);
            }
        }
    }
}

void Scene::AABBSphereCollisionSystem()
{
    LEV_PROFILE_FUNCTION();

    const auto view = m_Registry.group<>(entt::get<Transform, Rigidbody, BoxCollider>);
    const auto first = view.begin();
    const auto last = view.end();

    const auto viewB = m_Registry.group<>(entt::get<Transform, Rigidbody, SphereCollider>);
    const auto firstB = viewB.begin();
    const auto lastB = viewB.end();

    for (auto i = first; i != last; ++i)
    {
        auto [transform1, rigidbody1, colliderA] = view.get<Transform, Rigidbody, BoxCollider>(*i);
        for (auto j = firstB; j != lastB; ++j)
        {
            if (*i == *j) continue;

            auto [transform2, rigidbody2, colliderB] = viewB.get<Transform, Rigidbody, SphereCollider>(*j);

            if (!rigidbody1.enabled || !rigidbody2.enabled) continue;
            CollisionInfo info;

            if (Physics::HasAABBSphereIntersection(
                colliderA,
                transform1,
                colliderB,
                transform2,
                info))
            {
                Physics::HandleCollision(transform1, rigidbody1, transform2, rigidbody2, info.point);
                RegisterCollision(*i, *j);
            }
        }
    }
}

void Scene::AABBCollisionResolveSystem()
{
    LEV_PROFILE_FUNCTION();

    const auto group = m_Registry.group<Transform>(entt::get<Rigidbody, BoxCollider>);
    const auto first = group.begin();
    const auto last = group.end();

    for (auto i = first; i != last; ++i)
    {
        auto [transform1, rigidbody1, boxCollider1] = group.get<Transform, Rigidbody, BoxCollider>(*i);
        for (auto j = i + 1; j != last; ++j)
        {
            auto [transform2, rigidbody2, boxCollider2] = group.get<Transform, Rigidbody, BoxCollider>(*j);

            if (!rigidbody1.enabled || !rigidbody2.enabled) continue;

            CollisionInfo info;

            if (Physics::HasAABBIntersection(
	            boxCollider1,
	            transform1,
	            boxCollider2,
	            transform2,
                info))
            {
                Physics::HandleCollision(transform1, rigidbody1, transform2, rigidbody2, info.point);
                RegisterCollision(*i, *j);
            }
        }
    }
}


Ref<ShadowMapPass> s_ShadowMapPass;
Ref<SkyboxPass> s_SkyboxPass;
Ref<ParticlePass> s_ParticlePass;

void Scene::OnRender()
{
    LEV_PROFILE_FUNCTION();

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

    //Find Camera
    {
	    const auto group = m_Registry.group<>(entt::get<Transform, CameraComponent>);
        for (auto entity : group)
        {
            auto [transform, camera] = group.get<Transform, CameraComponent>(entity);

            if (camera.isMain)
            {
                using namespace entt::literals;
                m_Registry.ctx().emplace_as<Entity>("mainCameraEntity"_hs, Entity(entt::handle(m_Registry, entity)));
                mainCamera = &camera.camera;
                cameraTransform = transform.GetModel().Invert();
                cameraPosition = transform.GetWorldPosition();
                break;
            }
        }
    }

    //Forward Rendering

    if (!mainCamera) return;

    auto perspectiveViewProjectionMatrix = cameraTransform * mainCamera->GetPerspectiveProjection();
    RenderParams renderParams{ cameraPosition, *mainCamera, cameraTransform, perspectiveViewProjectionMatrix };

    if (!s_ShadowMapPass)
	    s_ShadowMapPass = CreateRef<ShadowMapPass>(m_Registry);

    if (!s_SkyboxPass)
        s_SkyboxPass = CreateRef<SkyboxPass>(m_Registry);

    if (!s_ParticlePass)
        s_ParticlePass = CreateRef<ParticlePass>(m_Registry);

    if (!RenderSettings::DeferredRendering)
    {
        LEV_PROFILE_SCOPE("Forward Rendering");

        DirectionalLightSystem();
        PointLightsSystem();

        {
            LEV_PROFILE_SCOPE("ShadowPass");

            auto success = s_ShadowMapPass->Begin(renderParams);
            if (success)
            {
                s_ShadowMapPass->Process(renderParams);
                s_ShadowMapPass->End(renderParams);
            }

           /* SceneCamera* lightCamera = nullptr;
            auto view = m_Registry.group<>(entt::get<Transform, DirectionalLightComponent, CameraComponent>);
            for (auto entity : view)
            {
                auto& camera = view.get<CameraComponent>(entity);
                lightCamera = &camera.camera;
            }

            if (lightCamera)
            {
                const auto cameraCascadeProjections = mainCamera->GetSplitPerspectiveProjections(RenderSettings::CascadeDistances, RenderSettings::CascadeCount);

                Renderer3D::BeginShadowPass(*lightCamera, cameraCascadeProjections, cameraTransform);

                MeshShadowSystem();

                Renderer3D::EndShadowPass();
            }*/
        }

        {
            LEV_PROFILE_SCOPE("RenderPass");
            Renderer3D::BeginScene(*mainCamera, cameraTransform, cameraPosition);

            Renderer3D::UpdateLights();

            MeshRenderSystem();

            //Render skybox
            s_SkyboxPass->Process(renderParams);

            Renderer3D::EndScene();

            s_ParticlePass->Process(renderParams);
        }
    }

    //Deferred Rendering
    if (RenderSettings::DeferredRendering)
    {
        LEV_PROFILE_SCOPE("Deferred Rendering");

        DirectionalLightSystem();

        {
            LEV_PROFILE_SCOPE("ShadowPass");

            /*SceneCamera* lightCamera = nullptr;
            auto view = m_Registry.group<>(entt::get<Transform, DirectionalLightComponent, CameraComponent>);
            for (auto entity : view)
            {
                auto& camera = view.get<CameraComponent>(entity);
                lightCamera = &camera.camera;
            }

            if (lightCamera)
            {
                const auto cameraCascadeProjections = mainCamera->GetSplitPerspectiveProjections(RenderSettings::CascadeDistances, RenderSettings::CascadeCount);

                Renderer3D::BeginShadowPass(*lightCamera, cameraCascadeProjections, cameraTransform);

                MeshShadowSystem();

                Renderer3D::EndShadowPass();
            }*/

            auto success = s_ShadowMapPass->Begin(renderParams);
            if (success)
            {
                s_ShadowMapPass->Process(renderParams);
                s_ShadowMapPass->End(renderParams);
            }
        }

	    //G-Buffer Pass
        {
            LEV_PROFILE_SCOPE("OpaquePass");

            Renderer3D::BeginDeferred(*mainCamera, cameraTransform, cameraPosition);

            MeshDeferredSystem();
        }
        
        //Lightning Pass
        {
            LEV_PROFILE_SCOPE("LightningPass");

            //Start Directional light pipeline
            Renderer3D::BeginDeferredDirLightningSubPass(*mainCamera);

            Renderer3D::BeginDeferredPositionalLightningSubPass(*mainCamera, cameraTransform, cameraPosition);
            //For each light
            auto view = m_Registry.group<>(entt::get<Transform, PointLightComponent>);
            for (auto entity : view)
            {
                auto [transform, light] = view.get<Transform, PointLightComponent>(entity);

                auto model = Matrix::CreateScale(light.Range, light.Range, light.Range)
            		* Matrix::CreateFromQuaternion(transform.GetWorldOrientation())
            		* Matrix::CreateTranslation(transform.GetWorldPosition());

                auto worldPosition = transform.GetWorldPosition();
                auto positionViewSpace = Vector4::Transform(Vector4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f), cameraTransform);
                Renderer3D::AddPointLights(positionViewSpace, worldPosition, light);

                // Start pipeline1
                Renderer3D::BeginDeferredPositionalLightningSubPass1(*mainCamera, cameraTransform, cameraPosition);
                // Render lights with sphere
                Renderer3D::RenderSphere(model);

                // Start pipeline2
                Renderer3D::BeginDeferredPositionalLightningSubPass2(*mainCamera, cameraTransform, cameraPosition);
                // Render lights with sphere
                Renderer3D::RenderSphere(model);
            }

            //Render skybox
            s_SkyboxPass->Process(renderParams);

            Renderer3D::EndDeferredLightningPass();
        }
    }
}

void Scene::DirectionalLightSystem()
{
    LEV_PROFILE_FUNCTION();

    auto view = m_Registry.group<>(entt::get<Transform, DirectionalLightComponent>);
    for (auto entity : view)
    {
        auto [transform, light] = view.get<Transform, DirectionalLightComponent>(entity);

        Renderer3D::SetDirLight(transform.GetForwardDirection(), light);
    }
}

void Scene::PointLightsSystem()
{
    LEV_PROFILE_FUNCTION();

    auto view = m_Registry.group<>(entt::get<Transform, PointLightComponent>);
    for (auto entity : view)
    {
        auto [transform, light] = view.get<Transform, PointLightComponent>(entity);

        Renderer3D::AddPointLights({}, transform.GetWorldPosition(), light);
    }
}

void Scene::MeshShadowSystem()
{
    LEV_PROFILE_FUNCTION();

    auto view = m_Registry.group<>(entt::get<Transform, MeshRendererComponent>);
    for (auto entity : view)
    {
        auto [transform, mesh] = view.get<Transform, MeshRendererComponent>(entity);
        if (mesh.castShadow)
			Renderer3D::DrawMesh(transform.GetModel(), mesh);
    }
}

void Scene::MeshDeferredSystem()
{
    LEV_PROFILE_FUNCTION();

    auto view = m_Registry.group<>(entt::get<Transform, MeshRendererComponent>);
    for (auto entity : view)
    {
        ShaderAssets::GBufferPass()->Bind();
        auto [transform, mesh] = view.get<Transform, MeshRendererComponent>(entity);
    	Renderer3D::DrawDeferredMesh(transform.GetModel(), mesh);
    }
}

void Scene::MeshRenderSystem()
{
    LEV_PROFILE_FUNCTION();

    auto view = m_Registry.group<>(entt::get<Transform, MeshRendererComponent>);
    for (auto entity : view)
    {
        auto [transform, mesh] = view.get<Transform, MeshRendererComponent>(entity);
        Renderer3D::DrawOpaqueMesh(transform.GetModel(), mesh);
    }
}

void Scene::OnViewportResized(const uint32_t width, const uint32_t height)
{
    LEV_PROFILE_FUNCTION();

    m_ViewportWidth = width;
    m_ViewportHeight = height;

    const auto view = m_Registry.view<CameraComponent>();
    for (auto entity : view)
    {
        auto& camera = view.get<CameraComponent>(entity);

        if (camera.fixedAspectRatio) continue;

        camera.camera.SetViewportSize(width, height);
    }
}

Entity Scene::CreateEntity(const std::string& name)
{
    LEV_PROFILE_FUNCTION();

    return CreateEntity(LevEngine::UUID(), name);
}

Entity Scene::CreateEntity(LevEngine::UUID uuid, const std::string& name)
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
    LEV_PROFILE_FUNCTION();

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