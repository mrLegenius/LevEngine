#include "Scene.h"

#include <Physics/Physics.h>

#include "Assets.h"
#include "Components/Components.h"
#include "Entity.h"
#include "OrbitCamera.h"
#include "Components/SkyboxRenderer.h"
#include "Kernel/Application.h"
#include "Physics/Components/Rigidbody.h"
#include "Renderer/Renderer3D.h"

struct CollisionInfo;
using namespace DirectX::SimpleMath;

Scene::~Scene()
{
    m_Registry.clear();
}

void Scene::OnUpdate(const float deltaTime)
{
	for (const auto system : m_UpdateSystems)
	{
        system->Update(deltaTime, m_Registry);
	}
}

static float dTOffset = 0;
int numCollisionFrames = 10;
void Scene::OnPhysics(const float deltaTime)
{
    constexpr float iterationDt = 1.0f / 60.0f; //Ideally we'll have 120 physics updates a second 
    dTOffset += deltaTime; //We accumulate time delta here - there might be remainders from previous frame!

    const int iterationCount = static_cast<int>(dTOffset / iterationDt); //And split it up here

    const float subDt = deltaTime / static_cast<float>(iterationCount);	//How many seconds per iteration do we get?

    UpdateVelocitySystem(deltaTime);

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
        }
        UpdatePositionSystem(subDt);
        dTOffset -= iterationDt;
    }
    ClearForcesSystem();	//Once we've finished with the forces, reset them to zero

    //UpdateCollisionList(); //Remove any old collisions
}

void Scene::OnLateUpdate(const float deltaTime)
{
    for (const auto system : m_LateUpdateSystems)
    {
        system->Update(deltaTime, m_Registry);
    }
}

void Scene::CollisionDetectionSystem()
{
    AABBCollisionResolveSystem();
    SphereCollisionSystem();
    AABBSphereCollisionSystem();
}

void Scene::HandleCollision(const CollisionInfo& info)
{
    Physics::HandleCollision(info);

    if (auto exist = allCollisions.find(info); exist == allCollisions.end())
    {
        info.framesLeft = numCollisionFrames;
        allCollisions.insert(info);
    }
    else
    {
        exist->framesLeft = numCollisionFrames - 1;
    }
}

void Scene::SphereCollisionSystem()
{
    const auto view = m_Registry.view<Transform, Rigidbody, SphereCollider>();
    const auto first = view.begin();
    const auto last = view.end();

    for (auto i = first; i != last; ++i)
    {
        auto [transform1, rigidbody1, colliderA] = view.get<Transform, Rigidbody, SphereCollider>(*i);
        for (auto j = first; j != last; ++j)
        {
            if (i == j) continue;

            auto [transform2, rigidbody2, colliderB] = view.get<Transform, Rigidbody, SphereCollider>(*j);

            if (!rigidbody1.enabled || !rigidbody2.enabled) continue;
            CollisionInfo info;

            info.transformA = &transform1;
            info.transformB = &transform2;

            info.rigidbodyA = &rigidbody1;
            info.rigidbodyB = &rigidbody2;

            if (Physics::HasSphereIntersection(
                colliderA,
                transform1,
                colliderB,
                transform2,
                info))
            {
                HandleCollision(info);
            }
        }
    }
}

void Scene::AABBSphereCollisionSystem()
{
    const auto view = m_Registry.view<Transform, Rigidbody, BoxCollider>();
    const auto first = view.begin();
    const auto last = view.end();

    const auto viewB = m_Registry.view<Transform, Rigidbody, SphereCollider>();
    const auto firstB = viewB.begin();
    const auto lastB = viewB.end();

    for (auto i = first; i != last; ++i)
    {
        auto [transform1, rigidbody1, colliderA] = view.get<Transform, Rigidbody, BoxCollider>(*i);
        for (auto j = firstB; j != lastB; ++j)
        {
            if (i == j) continue;

            auto [transform2, rigidbody2, colliderB] = viewB.get<Transform, Rigidbody, SphereCollider>(*j);

            if (!rigidbody1.enabled || !rigidbody2.enabled) continue;
            CollisionInfo info;

            info.transformA = &transform1;
            info.transformB = &transform2;

            info.rigidbodyA = &rigidbody1;
            info.rigidbodyB = &rigidbody2;

            if (Physics::HasAABBSphereIntersection(
                colliderA,
                transform1,
                colliderB,
                transform2,
                info))
            {
                HandleCollision(info);
            }
        }
    }
}

void Scene::AABBCollisionResolveSystem()
{
    const auto view = m_Registry.view<Transform, Rigidbody, BoxCollider>();
    const auto first = view.begin();
    const auto last = view.end();

    for (auto i = first; i != last; ++i)
    {
        auto [transform1, rigidbody1, boxCollider1] = view.get<Transform, Rigidbody, BoxCollider>(*i);
        for (auto j = first; j != last; ++j)
        {
            if (i == j) continue;

            auto [transform2, rigidbody2, boxCollider2] = view.get<Transform, Rigidbody, BoxCollider>(*j);

            if (!rigidbody1.enabled || !rigidbody2.enabled) continue;
            CollisionInfo info;

            info.transformA = &transform1;
            info.transformB = &transform2;

            info.rigidbodyA = &rigidbody1;
            info.rigidbodyB = &rigidbody2;

            if (Physics::HasAABBIntersection(
	            boxCollider1,
	            transform1,
	            boxCollider2,
	            transform2, 
                info))
            {
                HandleCollision(info);
            }
        }
    }
}

bool Scene::HasIntersection(const entt::entity& a, Transform& transformA, Rigidbody& rbA, const entt::entity& b, Transform& transformB, Rigidbody& rbB, CollisionInfo& collisionInfo)
{
    collisionInfo.transformA = &transformA;
    collisionInfo.transformB = &transformB;

    collisionInfo.rigidbodyA = &rbA;
    collisionInfo.rigidbodyB = &rbB;

    if (m_Registry.any_of<BoxCollider>(a) && m_Registry.any_of<BoxCollider>(b))
    {
        return Physics::HasAABBIntersection(
            m_Registry.get<BoxCollider>(a), 
            *collisionInfo.transformA, 
            m_Registry.get<BoxCollider>(b), 
            *collisionInfo.transformB, collisionInfo);
    }
    if (m_Registry.any_of<SphereCollider>(a) && m_Registry.any_of<SphereCollider>(b))
    {
        return Physics::HasSphereIntersection(
            m_Registry.get<SphereCollider>(a),
            *collisionInfo.transformA, 
            m_Registry.get<SphereCollider>(b),
            *collisionInfo.transformB, collisionInfo);
    }
    if (m_Registry.any_of<BoxCollider>(a) && m_Registry.any_of<SphereCollider>(b))
    {
        return Physics::HasAABBSphereIntersection(
            m_Registry.get<BoxCollider>(a), 
            *collisionInfo.transformA, 
            m_Registry.get<SphereCollider>(b),
            *collisionInfo.transformB, collisionInfo);
    }
    if (m_Registry.any_of<SphereCollider>(a) && m_Registry.any_of<BoxCollider>(b))
    {
        std::swap(collisionInfo.transformA, collisionInfo.transformB);
        std::swap(collisionInfo.rigidbodyA, collisionInfo.rigidbodyB);
        return Physics::HasAABBSphereIntersection(
            m_Registry.get<BoxCollider>(b), 
            *collisionInfo.transformB, 
            m_Registry.get<SphereCollider>(a), 
            *collisionInfo.transformA, collisionInfo);
    }

    return false;
}

void Scene::UpdateVelocitySystem(const float deltaTime)
{
    auto view = m_Registry.view<Transform, Rigidbody>();
    for (auto entity : view)
    {
        auto [transform, rigidbody] = view.get<Transform, Rigidbody>(entity);

        if (rigidbody.bodyType != BodyType::Dynamic || !rigidbody.enabled) continue;

        const auto inverseMass = rigidbody.GetInverseMass();
        Vector3 acceleration = rigidbody.force * inverseMass;

        if (inverseMass > 0)
            acceleration += gravity * rigidbody.gravityScale;

        rigidbody.velocity += acceleration * deltaTime;

        rigidbody.UpdateInertiaTensor(transform);

        const Vector3 angularAcceleration = Vector3::Transform(rigidbody.torque, rigidbody.GetInertiaTensor());
        //const Vector3 angularAcceleration = torque;

        rigidbody.angularVelocity += angularAcceleration * deltaTime;
    }
}

void Scene::UpdatePositionSystem(const float deltaTime)
{
	const auto view = m_Registry.view<Transform, Rigidbody>();
    for (const auto entity : view)
    {
        auto [transform, rigidbody] = view.get<Transform, Rigidbody>(entity);

        if (rigidbody.bodyType != BodyType::Dynamic || !rigidbody.enabled) return;

        //Linear movement
        Vector3 position = transform.GetLocalPosition();
        position += rigidbody.velocity * deltaTime;
        transform.SetLocalPosition(position);

        // Linear Damping
        const float dampingFactor = 1.0f - rigidbody.damping;
        const float frameDamping = powf(dampingFactor, deltaTime);
        rigidbody.velocity *= frameDamping;

        //Angular movement
        Quaternion orientation = transform.GetLocalOrientation();

        orientation += orientation * Quaternion(rigidbody.angularVelocity * deltaTime * 0.5f, 0.0f);
        orientation.Normalize();

        transform.SetLocalRotation(orientation);

        // Angular Damping
        const float angularDampingFactor = 1.0f - rigidbody.angularDamping;
        const float angularFrameDamping = powf(angularDampingFactor, deltaTime);
        rigidbody.angularVelocity *= angularFrameDamping;
    }
}

void Scene::ClearForcesSystem()
{
    const auto view = m_Registry.view<Rigidbody>();
    for (const auto entity : view)
    {
        auto& rigidbody = view.get<Rigidbody>(entity);

        rigidbody.ClearForces();
    }
}

void Scene::OnRender()
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
                using namespace entt::literals;
                m_Registry.ctx().emplace_as<Entity>("mainCameraEntity"_hs, Entity(entt::handle(m_Registry, entity)));
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

void Scene::OnViewportResized(const uint32_t width, const uint32_t height)
{
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
    return CreateEntity(LevEngine::UUID(), name);
}

Entity Scene::CreateEntity(LevEngine::UUID uuid, const std::string& name)
{
    auto entity = Entity(entt::handle{ m_Registry, m_Registry.create() });
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
            return Entity(entt::handle(m_Registry, entity));
    }
    return { };
}