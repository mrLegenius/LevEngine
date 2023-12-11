#include "levpch.h"
#include "Physics.h"

#include "Physics/Components/Rigidbody.h"
#include "Renderer/DebugRender/DebugRender.h"
#include "Physics/PhysicsUtils.h"

constexpr auto DEFAULT_PVD_HOST = "127.0.0.1";
constexpr auto DEFAULT_PVD_PORT = 5425;
constexpr auto DEFAULT_PVD_CONNECT_TIMEOUT = 10;
constexpr auto DEFAULT_NUMBER_CPU_THREADS = 2;

namespace LevEngine
{
    Scope<Physics> Physics::Create()
    {
        return CreateScope<Physics>();
    }

    void Physics::ClearAccumulator()
    {
        m_Accumulator = 0.0f;
    }
    

    
    void Physics::Initialize()
    {
        m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

        if (m_IsPVDEnabled)
        {
            m_Pvd = PxCreatePvd(*m_Foundation);
            physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(DEFAULT_PVD_HOST, DEFAULT_PVD_PORT, DEFAULT_PVD_CONNECT_TIMEOUT);
            m_Pvd->connect(*transport,physx::PxPvdInstrumentationFlag::eALL);
        }
        
        m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale, true, m_Pvd);
        
        physx::PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
        sceneDesc.gravity = PhysicsUtils::FromVector3ToPxVec3(m_Gravity);
        m_Dispatcher = physx::PxDefaultCpuDispatcherCreate(DEFAULT_NUMBER_CPU_THREADS);
        sceneDesc.cpuDispatcher	= m_Dispatcher;
        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        m_Scene = m_Physics->createScene(sceneDesc);

        if (m_IsDebugRenderEnabled)
        {
            m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
            m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);
            m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
            m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_MASS_AXES, 1.0f);
        }
        
        if (m_IsPVDEnabled)
        {
            if (physx::PxPvdSceneClient* pvdClient = m_Scene->getScenePvdClient())
            {
                pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
                pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
                pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
            }
        }
    }
    
    Physics::Physics()
    {
        Initialize();
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
                                            Color(0.8f, 0.8f, 0.8f, 1.0f));
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
        if (!StepPhysics(deltaTime)) return;

        m_PhysicsUpdate.UpdateTransforms(registry);
        m_PhysicsUpdate.UpdateConstantForces(registry);
        
        if (!m_IsDebugRenderEnabled) return;
        
        DrawDebugLines();
    }


    
    void Physics::Reset()
    {
        PX_RELEASE(m_Scene)
        PX_RELEASE(m_Dispatcher)
        PX_RELEASE(m_Physics)
        if (m_Pvd)
        {
            physx::PxPvdTransport* transport = m_Pvd->getTransport();
            PX_RELEASE(m_Pvd)
            PX_RELEASE(transport)
        }
        PX_RELEASE(m_Foundation)
    }
    
    Physics::~Physics()
    {
        Reset();
    }

    
    
    physx::PxScene* Physics::GetScene() const
    {
        return m_Scene;
    }

    physx::PxPhysics* Physics::GetPhysics() const
    {
        return m_Physics;
    }

    void PhysicsUpdate::UpdateTransforms(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Transform, Rigidbody>();
        for (const auto entity : rigidbodyView)
        {
            auto [rigidbodyTransform, rigidbody] = rigidbodyView.get<Transform, Rigidbody>(entity);
        
            const physx::PxTransform actorPose = rigidbody.GetActor()->getGlobalPose();
            rigidbodyTransform.SetWorldRotation(PhysicsUtils::FromPxQuatToQuaternion(actorPose.q));
            rigidbodyTransform.SetWorldPosition(PhysicsUtils::FromPxVec3ToVector3(actorPose.p));
        
            rigidbody.SetTransformScale(rigidbodyTransform.GetWorldScale());
        }
    }

    void PhysicsUpdate::UpdateConstantForces(entt::registry& registry)
    {
        const auto constantForceView = registry.view<Rigidbody, ConstantForce>();
        for (const auto entity : constantForceView)
        {
            auto [constantForceRigidbody, constantForce] = constantForceView.get<Rigidbody, ConstantForce>(entity);

            if (constantForceRigidbody.GetActor() != NULL)
            {
                constantForceRigidbody.AddForce(constantForce.GetForce(), Rigidbody::ForceMode::Force);
                constantForceRigidbody.AddTorque(constantForce.GetTorque(), Rigidbody::ForceMode::Force);
            }
        }
    }

}