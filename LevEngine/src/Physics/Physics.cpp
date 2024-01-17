#include "levpch.h"
#include "Physics.h"
#include "PhysicsUtils.h"
#include "Components/CharacterController.h"
#include "Renderer/DebugRender/DebugRender.h"

namespace LevEngine
{
    constexpr auto DEFAULT_PVD_HOST = "127.0.0.1";
    constexpr auto DEFAULT_PVD_PORT = 5425;
    constexpr auto DEFAULT_PVD_CONNECT_TIMEOUT = 10;
    constexpr auto DEFAULT_NUMBER_CPU_THREADS = 2;

    constexpr auto DEFAULT_STATIC_FRICTION = 0.5f;
    constexpr auto DEFAULT_DYNAMIC_FRICTION = 0.5f;
    constexpr auto DEFAULT_RESTITUTION = 0.6f;
    
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
        sceneDesc.gravity = PhysicsUtils::FromVector3ToPxVec3(m_GravityScale);
        sceneDesc.cpuDispatcher	= m_Dispatcher;
        sceneDesc.filterShader = ContactReportCallback::ContactReportFilterShader;
        sceneDesc.simulationEventCallback = &m_ContactReportCallback;
        m_Scene = m_Physics->createScene(sceneDesc);
        m_ControllerManager = PxCreateControllerManager(*m_Scene);
        m_ControllerManager->setDebugRenderingFlags(physx::PxControllerDebugRenderFlag::eALL);
        
        if (m_IsDebugRenderEnabled)
        {
            m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
            m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);
            m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
        }

        m_ActorEntityMap.clear();
    }

    physx::PxRigidActor* Physics::CreateStaticActor(const Entity entity)
    {
        const auto& transform = entity.GetComponent<Transform>();
        physx::PxRigidActor* actor = m_Physics->createRigidStatic(PhysicsUtils::FromTransformToPxTransform(transform));
        m_Scene->addActor(*actor);

        m_ActorEntityMap.insert({actor, entity});

        return actor;
    }

    physx::PxRigidActor* Physics::CreateDynamicActor(const Entity entity)
    {
        const auto& transform = entity.GetComponent<Transform>();
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
    
    physx::PxMaterial* Physics::CreatePhysicMaterial(
        float staticFriction = DEFAULT_STATIC_FRICTION,
        float dynamicFriction = DEFAULT_DYNAMIC_FRICTION,
        float restitution = DEFAULT_RESTITUTION
    ) const
    {
        const auto material =
            m_Physics->createMaterial(
                staticFriction,
                dynamicFriction,
                restitution
            );
        
        return material;
    }

    physx::PxShape* Physics::CreateSphere(const float radius) const
    {
        const auto material = CreatePhysicMaterial();
        
        const auto sphere =
            m_Physics->createShape(
                physx::PxSphereGeometry(radius),
                *material,
                true
            );

        material->release();
        
        return sphere;
    }

    physx::PxShape* Physics::CreateCapsule(const float radius, const float halfHeight) const
    {
        const auto material = CreatePhysicMaterial();
        
        const auto capsule =
            m_Physics->createShape(
                physx::PxCapsuleGeometry(radius, halfHeight),
                *material,
                true
            );

        material->release();
        
        return capsule;
    }

    physx::PxShape* Physics::CreateBox(const Vector3 halfExtents) const
    {
        const auto material = CreatePhysicMaterial();
        
        const auto box =
            m_Physics->createShape(
                physx::PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(halfExtents)),
                *material,
                true
            );

        material->release();
        
        return box;
    }

    physx::PxController* Physics::CreateCapsuleController(Entity entity, const float radius, const float height)
    {
        const auto material = CreatePhysicMaterial();

        const auto controllerPosition =
            entity.GetComponent<Transform>().GetWorldPosition() +
                entity.GetComponent<CharacterController>().GetCenterOffset();
        
        physx::PxCapsuleControllerDesc desc;
        desc.height = height;
        desc.radius = radius;
        desc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
        desc.material = material;
        desc.position = PhysicsUtils::FromVector3ToPxExtendedVec3(controllerPosition);
        desc.scaleCoeff = 1.0f;
        physx::PxController* controller = m_ControllerManager->createController(desc);

        material->release();
        
        const auto actor = controller->getActor();
        m_ActorEntityMap.insert({actor, entity});
        
        return controller;
    }

    void Physics::RemoveController(physx::PxController* controller)
    {
        const auto actor = controller->getActor();
        m_ActorEntityMap.erase(actor);
        
        PX_RELEASE(controller)
    }

    Entity Physics::GetEntityByActor(const physx::PxActor* actor) const
    {
        return m_ActorEntityMap.at(actor);
    }

    Vector3 Physics::GetGravity() const
    {
        return m_GravityScale;
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
    
    void Physics::DrawDebugLines()
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
        m_PhysicsUpdate.UpdateControllerGroundFlag(registry);
        m_PhysicsUpdate.ApplyControllerGravity(registry, deltaTime);
        m_PhysicsUpdate.ApplyKinematicTargets(registry);
        m_PhysicsUpdate.UpdateConstantForces(registry);

        if (!m_IsDebugRenderEnabled) return;
        DrawDebugLines();
    }

    RaycastHit Physics::Raycast(
        const Vector3 origin,
        const Vector3 direction,
        const float maxDistance,
        const bool isDebugDrawn,
        const FilterLayer& layerMask
    ) const
    {
        if (maxDistance < 0.0f) return RaycastHit {};
        
        physx::PxRaycastBuffer buffer;
        const auto outputFlags = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;
        physx::PxQueryFilterData filterData;
        const auto degreeOfTwo = std::pow(2, static_cast<physx::PxU32>(layerMask));
        filterData.data.word0 = degreeOfTwo;
        
        const bool isHitSuccessfully =
            m_Scene->raycast(
                PhysicsUtils::FromVector3ToPxVec3(origin),
                PhysicsUtils::FromVector3ToPxVec3(direction),
                maxDistance,
                buffer,
                outputFlags,
                filterData
            );
        
        RaycastHit hitResult {};
        if (isHitSuccessfully)
        {
            hitResult.IsSuccessful = true;
            hitResult.Entity = GetEntityByActor(buffer.block.actor);
            hitResult.Point = PhysicsUtils::FromPxVec3ToVector3(buffer.block.position);
            hitResult.Normal = PhysicsUtils::FromPxVec3ToVector3(buffer.block.normal);
            hitResult.Distance = buffer.block.distance;

            if (isDebugDrawn)
            {
                DebugRender::DrawLine(origin, hitResult.Point, Color::Green, 3.0f);
                DebugRender::DrawPoint(hitResult.Point, Color::Red, 3.0f);
            }
        }

        return hitResult;
    }

    RaycastHit Physics::SphereCast(
        const float radius,
        const Vector3 origin,
        const Vector3 direction,
        const float maxDistance,
        const bool isDebugDrawn,
        const FilterLayer& layerMask
    ) const
    {
        if (maxDistance < 0.0f) return RaycastHit {};

        const auto sphereShape =
            physx::PxSphereGeometry(radius);
        const auto initialPose =
            physx::PxTransform(PhysicsUtils::FromVector3ToPxVec3(origin));

        physx::PxSweepBuffer buffer;
        const auto outputFlags = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;
        physx::PxQueryFilterData filterData;
        const auto degreeOfTwo = std::pow(2, static_cast<physx::PxU32>(layerMask));
        filterData.data.word0 = degreeOfTwo;
        
        const bool isHitSuccessfully =
            m_Scene->sweep(
                sphereShape,
                initialPose,
                PhysicsUtils::FromVector3ToPxVec3(direction),
                maxDistance,
                buffer,
                outputFlags,
                filterData
            );
        
        RaycastHit hitResult {};
        if (isHitSuccessfully)
        {
            hitResult.IsSuccessful = true;
            hitResult.Entity = GetEntityByActor(buffer.block.actor);
            hitResult.Point = PhysicsUtils::FromPxVec3ToVector3(buffer.block.position);
            hitResult.Normal = PhysicsUtils::FromPxVec3ToVector3(buffer.block.normal);
            hitResult.Distance = buffer.block.distance;

            if (isDebugDrawn)
            {
                const auto position = origin + direction * hitResult.Distance;
                DebugRender::DrawWireSphere(position, radius, Color::Green, 3.0f);
                DebugRender::DrawPoint(hitResult.Point, Color::Red, 3.0f);
            }
        }
        
        return hitResult;
    }

    RaycastHit Physics::CapsuleCast(
        const float radius,
        const float halfHeight,
        const Vector3 origin,
        const Quaternion orientation,
        const Vector3 direction,
        const float maxDistance,
        const bool isDebugDrawn,
        const FilterLayer& layerMask
    ) const
    {
        if (maxDistance < 0.0f) return RaycastHit {};
        
        const auto capsuleShape =
            physx::PxCapsuleGeometry(radius, halfHeight);
        const auto initialPose =
            physx::PxTransform(PhysicsUtils::FromVector3ToPxVec3(origin), PhysicsUtils::FromQuaternionToPxQuat(orientation));
        
        physx::PxSweepBuffer buffer;
        const auto outputFlags = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;
        physx::PxQueryFilterData filterData;
        const auto degreeOfTwo = std::pow(2, static_cast<physx::PxU32>(layerMask));
        filterData.data.word0 = degreeOfTwo;
        
        const bool isHitSuccessfully =
            m_Scene->sweep(
                capsuleShape,
                initialPose,
                PhysicsUtils::FromVector3ToPxVec3(direction),
                maxDistance,
                buffer,
                outputFlags,
                filterData
            );

        RaycastHit hitResult {};
        if (isHitSuccessfully)
        {
            hitResult.IsSuccessful = true;
            hitResult.Entity = GetEntityByActor(buffer.block.actor);
            hitResult.Point = PhysicsUtils::FromPxVec3ToVector3(buffer.block.position);
            hitResult.Normal = PhysicsUtils::FromPxVec3ToVector3(buffer.block.normal);
            hitResult.Distance = buffer.block.distance;

            if (isDebugDrawn)
            {
                const auto position = origin + direction * hitResult.Distance;
                const auto model =
                    Matrix::CreateFromQuaternion(orientation) * Matrix::CreateTranslation(position);
                DebugRender::DrawWireCapsule(model, halfHeight, radius, Color::Green, 3.0f);
                DebugRender::DrawPoint(hitResult.Point, Color::Red, 3.0f);
            }
        }
        
        return hitResult;
    }

    RaycastHit Physics::BoxCast(
        const Vector3 halfExtents,
        const Vector3 origin,
        const Quaternion orientation,
        const Vector3 direction,
        const float maxDistance,
        const bool isDebugDrawn,
        const FilterLayer& layerMask
    ) const
    {
        if (maxDistance < 0.0f) return RaycastHit {};

        const auto boxShape =
            physx::PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(halfExtents));
        const auto initialPose =
            physx::PxTransform(PhysicsUtils::FromVector3ToPxVec3(origin), PhysicsUtils::FromQuaternionToPxQuat(orientation));
        
        physx::PxSweepBuffer buffer;
        const auto outputFlags = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL;
        physx::PxQueryFilterData filterData;
        const auto degreeOfTwo = std::pow(2, static_cast<physx::PxU32>(layerMask));
        filterData.data.word0 = degreeOfTwo;
        
        const bool isHitSuccessfully =
            m_Scene->sweep(
                boxShape,
                initialPose,
                PhysicsUtils::FromVector3ToPxVec3(direction),
                maxDistance,
                buffer,
                outputFlags,
                filterData
            );
        
        RaycastHit hitResult {};
        if (isHitSuccessfully)
        {
            hitResult.IsSuccessful = true;
            hitResult.Entity = GetEntityByActor(buffer.block.actor);
            hitResult.Point = PhysicsUtils::FromPxVec3ToVector3(buffer.block.position);
            hitResult.Normal = PhysicsUtils::FromPxVec3ToVector3(buffer.block.normal);
            hitResult.Distance = buffer.block.distance;

            if (isDebugDrawn)
            {
                const auto position =
                    origin + direction * hitResult.Distance;
                const auto transformModel =
                    Matrix::CreateScale(halfExtents * 2) *
                        Matrix::CreateFromQuaternion(orientation) *
                            Matrix::CreateTranslation(position);
                DebugRender::DrawWireCube(transformModel, Color::Green, 3.0f);
                DebugRender::DrawPoint(hitResult.Point, Color::Red, 3.0f);
            }
        }
        
        return hitResult;
    }
}