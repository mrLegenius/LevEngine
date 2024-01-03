#include "levpch.h"
#include "Physics.h"

#include "PhysicsUtils.h"
#include "Components/Controller.h"
#include "Physics/Components/Rigidbody.h"
#include "Renderer/DebugRender/DebugRender.h"

constexpr auto DEFAULT_PVD_HOST = "127.0.0.1";
constexpr auto DEFAULT_PVD_PORT = 5425;
constexpr auto DEFAULT_PVD_CONNECT_TIMEOUT = 10;
constexpr auto DEFAULT_NUMBER_CPU_THREADS = 2;

constexpr Vector3 DEFAULT_GRAVITY_SCALE = Vector3(0.0f, -9.81f, 0.0f);

namespace LevEngine
{
    Scope<Physics> Physics::Create()
    {
        return CreateScope<Physics>();
    }
    
    void Physics::Initialize()
    {
        m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
        m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, physx::PxTolerancesScale());
        m_Dispatcher = physx::PxDefaultCpuDispatcherCreate(DEFAULT_NUMBER_CPU_THREADS);
        
        ResetPhysicsScene();
    }
    
    Physics::Physics()
    {
        Initialize();
    }

    void Physics::Reset()
    {
        PX_RELEASE(m_ControllerManager)
        PX_RELEASE(m_Scene)
        PX_RELEASE(m_Dispatcher)
        PX_RELEASE(m_Physics)
        PX_RELEASE(m_Foundation)
    }
    
    Physics::~Physics()
    {
        Reset();
    }

    void Physics::ResetPhysicsScene()
    {
        PX_RELEASE(m_ControllerManager)
        PX_RELEASE(m_Scene)
        
        physx::PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
        sceneDesc.gravity = PhysicsUtils::FromVector3ToPxVec3(DEFAULT_GRAVITY_SCALE);
        sceneDesc.cpuDispatcher	= m_Dispatcher;
        sceneDesc.filterShader = ContactReportCallback::ContactReportFilterShader;
        sceneDesc.simulationEventCallback = &m_ContactReportCallback;
        m_Scene = m_Physics->createScene(sceneDesc);
        m_ControllerManager = PxCreateControllerManager(*m_Scene);
        
        if (m_IsDebugRenderEnabled)
        {
            m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
            m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);
            m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
        }
    }

    physx::PxRigidActor* Physics::CreateStaticActor(const Entity entity)
    {
        const auto transform = entity.GetComponent<Transform>();
        physx::PxRigidActor* actor = m_Physics->createRigidStatic(PhysicsUtils::FromTransformToPxTransform(transform));
        m_Scene->addActor(*actor);

        m_ActorEntityMap.insert({actor, entity});

        return actor;
    }

    physx::PxRigidActor* Physics::CreateDynamicActor(const Entity entity)
    {
        const auto transform = entity.GetComponent<Transform>();
        physx::PxRigidActor* actor = m_Physics->createRigidDynamic(PhysicsUtils::FromTransformToPxTransform(transform));
        m_Scene->addActor(*actor);

        m_ActorEntityMap.insert({actor, entity});

        return actor;
    }

    void Physics::RemoveActor(physx::PxActor* actor)
    {
        m_ActorEntityMap.erase(actor);
        
        m_Scene->removeActor(*actor);
        PX_RELEASE(actor)
    }
    
    physx::PxMaterial* Physics::CreateMaterial(float staticFriction, float dynamicFriction, float restitution) const
    {
        const auto material =
            m_Physics->createMaterial(
                staticFriction,
                dynamicFriction,
                restitution
            );
        
        return material;
    }

    physx::PxShape* Physics::CreateSphere(const float radius, const physx::PxMaterial* material) const
    {
        const auto sphere =
            m_Physics->createShape(
                physx::PxSphereGeometry(radius),
                *material,
                true
            );
        
        return sphere;
    }

    physx::PxShape* Physics::CreateCapsule(const float radius, const float halfHeight, const physx::PxMaterial* material) const
    {
        const auto capsule =
            m_Physics->createShape(
                physx::PxCapsuleGeometry(radius, halfHeight),
                *material,
                true
            );
        
        return capsule;
    }

    physx::PxShape* Physics::CreateBox(const Vector3 halfExtents, const physx::PxMaterial* material) const
    {
        const auto box =
            m_Physics->createShape(
                physx::PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(halfExtents)),
                *material,
                true
            );
        
        return box;
    }

    physx::PxController* Physics::CreateCapsuleController(
        const Entity entity,
        const float radius,
        const float height,
        const Controller::ClimbingMode climbingMode,
        physx::PxMaterial* material)
    {
        auto transform = entity.GetComponent<Transform>();
        physx::PxCapsuleControllerDesc desc;
        desc.position = PhysicsUtils::FromVector3ToPxExtendedVec3(transform.GetWorldPosition());
        desc.radius = radius;
        desc.height = height;
        desc.climbingMode = static_cast<physx::PxCapsuleClimbingMode::Enum>(static_cast<int>(climbingMode));
        desc.material = material;
        desc.upDirection = PhysicsUtils::FromVector3ToPxVec3(Vector3::Right);
        physx::PxController* controller = m_ControllerManager->createController(desc);
        
        const auto actor = controller->getActor();
        m_ActorEntityMap.insert({actor, entity});
        
        // turn the capsule into an upright position
        physx::PxShape* collider[1];
        const auto nbColliders = actor->getNbShapes();
        actor->getShapes(collider, nbColliders);
        collider[0]->setLocalPose(
            physx::PxTransform(
                PhysicsUtils::FromQuaternionToPxQuat(
                    Quaternion::CreateFromAxisAngle(Vector3::Forward, Math::ToRadians(90.0f))
                )
            )
        );
        
        return controller;
    }

    void Physics::RemoveController(physx::PxController* controller)
    {
        const auto actor = controller->getActor();
        m_ActorEntityMap.erase(actor);
        
        PX_RELEASE(controller)
    }

    Entity Physics::GetEntityByActor(physx::PxActor* actor) const
    {
        return m_ActorEntityMap.at(actor);
    }

    Vector3 Physics::GetGravity() const
    {
        return PhysicsUtils::FromPxVec3ToVector3(m_Scene->getGravity());
    }
    
    void Physics::ClearAccumulator()
    {
        m_Accumulator = 0.0f;
    }
    
    bool Physics::IsAdvanced(const float deltaTime)
    {
        m_Accumulator += deltaTime;
        
        if (m_Accumulator < m_StepSize) return false;
        
        m_Accumulator -= m_StepSize;
        m_Scene->simulate(m_StepSize);
        
        return true;
    }
    
    bool Physics::StepPhysics(const float deltaTime)
    {
        if (!IsAdvanced(deltaTime)) return false;

        m_Scene->fetchResults(true);
        
        return true;
    }
    
    void Physics::DrawDebugLines() const
    {
        const physx::PxRenderBuffer& rb = m_Scene->getRenderBuffer();
        for (size_t i = 0; i < rb.getNbLines(); i++)
        {
            const physx::PxDebugLine& line = rb.getLines()[i];
            DebugRender::DrawLine(Vector3(line.pos0.x, line.pos0.y, line.pos0.z),
                                    Vector3(line.pos1.x, line.pos1.y, line.pos1.z),
                                        Color(1.0f, 0.0f, 0.0f, 1.0f));
        }
        for (size_t i = 0; i < rb.getNbPoints(); i++)
        {
            const physx::PxDebugPoint& point = rb.getPoints()[i];
            DebugRender::DrawPoint(Vector3(point.pos.x, point.pos.y, point.pos.z),
                                            Color(1.0f, 0.0f, 0.0f, 1.0f));
        }
        for (size_t i = 0; i < rb.getNbTriangles(); i++)
        {
            const physx::PxDebugTriangle& tri = rb.getTriangles()[i];
            DebugRender::DrawLine(Vector3(tri.pos0.x, tri.pos0.y, tri.pos0.z),
                                    Vector3(tri.pos1.x, tri.pos1.y, tri.pos1.z),
                                        Color(1.0f, 0.0f, 0.0f, 1.0f));
            DebugRender::DrawLine(Vector3(tri.pos1.x, tri.pos1.y, tri.pos1.z),
                                    Vector3(tri.pos2.x, tri.pos2.y, tri.pos2.z),
                                        Color(1.0f, 0.0f, 0.0f, 1.0f));
            DebugRender::DrawLine(Vector3(tri.pos0.x, tri.pos0.y, tri.pos0.z),
                                    Vector3(tri.pos2.x, tri.pos2.y, tri.pos2.z),
                                        Color(1.0f, 0.0f, 0.0f, 1.0f));
        }
    }
    
    void Physics::Process(entt::registry& registry, float deltaTime)
    {
        m_PhysicsUpdate.ClearBuffers(registry);
        
        if (!StepPhysics(deltaTime)) return;
        m_PhysicsUpdate.UpdateTransforms(registry);
        m_PhysicsUpdate.ApplyKinematicTargets(registry);
        m_PhysicsUpdate.UpdateConstantForces(registry);
        
        if (!m_IsDebugRenderEnabled) return;
        DrawDebugLines();
    }
}