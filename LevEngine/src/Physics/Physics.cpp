#include "levpch.h"
#include "Physics.h"

#include "Physics/Components/Rigidbody.h"
#include "Renderer/DebugRender/DebugRender.h"
#include "Physics/PhysicsUtils.h"

constexpr auto DEFAULT_PVD_HOST = "127.0.0.1";
constexpr auto DEFAULT_PVD_PORT = 5425;
constexpr auto DEFAULT_PVD_CONNECT_TIMEOUT = 10;
constexpr auto DEFAULT_NUMBER_CPU_THREADS = 2;

constexpr Vector3 DEFAULT_GRAVITY_SCALE = {0.0f, -9.81f, 0.0f};

namespace LevEngine
{
    Physics Physics::s_Physics;
    
    void Physics::Initialize()
    {
        m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

        if (s_IsPVDEnabled)
        {
            m_Pvd = PxCreatePvd(*m_Foundation);
            PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(DEFAULT_PVD_HOST, DEFAULT_PVD_PORT, DEFAULT_PVD_CONNECT_TIMEOUT);
            m_Pvd->connect(*transport,PxPvdInstrumentationFlag::eALL);
        }
        
        m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale, true, m_Pvd);
        
        PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
        sceneDesc.gravity = PhysicsUtils::FromVector3ToPxVec3(DEFAULT_GRAVITY_SCALE);
        m_Dispatcher = PxDefaultCpuDispatcherCreate(DEFAULT_NUMBER_CPU_THREADS);
        sceneDesc.cpuDispatcher	= m_Dispatcher;
        sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
        m_Scene = m_Physics->createScene(sceneDesc);

        if (s_IsDebugRenderEnabled)
        {
            m_Scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
            m_Scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
            m_Scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
        }
        
        if (s_IsPVDEnabled)
        {
            if (PxPvdSceneClient* pvdClient = m_Scene->getScenePvdClient())
            {
                pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
                pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
                pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
            }
        }
    }
    Physics::Physics()
    {
        Initialize();
    }
    
    bool Physics::Advance(float deltaTime)
    {
        s_Accumulator += deltaTime;
        
        if (s_Accumulator < s_StepSize) return false;
        
        s_Accumulator -= s_StepSize;
        s_Physics.m_Scene->simulate(s_StepSize);
        
        return true;
    }
    void Physics::StepPhysics(float deltaTime)
    {
        if (Advance(deltaTime))
        {
            s_Physics.m_Scene->fetchResults(true);
        }
    }
    void Physics::UpdateTransforms(entt::registry& registry)
    {
        const auto view = registry.view<Rigidbody, Transform>();
        for (const auto entity : view)
        {
            auto [transform, rigidbody] = view.get<Transform, Rigidbody>(entity);
            const PxTransform actorPose = rigidbody.GetRigidbody()->getGlobalPose();
            transform.SetWorldRotation(PhysicsUtils::FromPxQuatToQuaternion(actorPose.q));
            transform.SetWorldPosition(PhysicsUtils::FromPxVec3ToVector3(actorPose.p));
        }
    }
    void Physics::DrawDebugLines()
    {
        const PxRenderBuffer& rb = s_Physics.m_Scene->getRenderBuffer();
        auto a = rb.getNbLines();
        for (auto i = 0; i < rb.getNbLines(); i++)
        {
            const PxDebugLine& line = rb.getLines()[i];
            DebugRender::DrawLine(Vector3(line.pos0.x, line.pos0.y, line.pos0.z),
                                    Vector3(line.pos1.x, line.pos1.y, line.pos1.z),
                                        Color(1.0f, 0.0f, 0.0f, 1.0f));
        }
        for (auto i = 0; i < rb.getNbPoints(); i++)
        {
            const PxDebugPoint& point = rb.getPoints()[i];
            DebugRender::DrawPoint(Vector3(point.pos.x, point.pos.y, point.pos.z),
                                            Color(0.8f, 0.8f, 0.8f, 1.0f));
        }
        auto b = rb.getNbTriangles();
        for (auto i = 0; i < rb.getNbTriangles(); i++)
        {
            const PxDebugTriangle& tri = rb.getTriangles()[i];
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
        StepPhysics(deltaTime);
        
        UpdateTransforms(registry);

        if (s_IsDebugRenderEnabled)
        {
            DrawDebugLines();
        }
    }
    
    void Physics::Deinitialize()
    {
        PX_RELEASE(m_Scene)
        PX_RELEASE(m_Dispatcher)
        PX_RELEASE(m_Physics)
        if (m_Pvd)
        {
            PxPvdTransport* transport = m_Pvd->getTransport();
            PX_RELEASE(m_Pvd)
            PX_RELEASE(transport)
        }
        PX_RELEASE(m_Foundation)
    }
    Physics::~Physics()
    {
        Deinitialize();
    }

    Physics& Physics::GetInstance()
    {
        return s_Physics;
    }
    
    PxScene* Physics::GetScene() const
    {
        return m_Scene;
    }

    PxPhysics* Physics::GetPhysics() const
    {
        return m_Physics;
    }

    Vector3 Physics::GetGravity() const
    {
        const auto gravity = m_Scene->getGravity();
        return PhysicsUtils::FromPxVec3ToVector3(gravity);
    }

    void Physics::SetGravity(const Vector3 newGravity)
    {
        const auto gravity = PhysicsUtils::FromVector3ToPxVec3(newGravity);
        m_Scene->setGravity(gravity);
    }
}

