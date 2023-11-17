#include "levpch.h"
#include "PhysicsBase.h"

#include "PhysicsRigidbody.h"
#include "Renderer/DebugRender/DebugRender.h"
#include "Physics/PhysX/PhysicsUtils.h"

constexpr auto DEFAULT_PVD_HOST = "127.0.0.1";
constexpr auto DEFAULT_PVD_PORT = 5425;
constexpr auto DEFAULT_PVD_CONNECT_TIMEOUT = 10;
constexpr auto DEFAULT_NUMBER_CPU_THREADS = 2;

constexpr Vector3 DEFAULT_GRAVITY_SCALE = {0.0f, -9.81f, 0.0f};

namespace LevEngine
{
    PhysicsBase PhysicsBase::s_PhysicsBase;
    
    void PhysicsBase::InitPhysics()
    {
        m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

        if (s_IsPVDEnabled)
        {
            m_Pvd = PxCreatePvd(*m_Foundation);
            PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(DEFAULT_PVD_HOST, DEFAULT_PVD_PORT, DEFAULT_PVD_CONNECT_TIMEOUT);
            m_Pvd->connect(*transport,PxPvdInstrumentationFlag::eALL);
        }
        
        m_Factory = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale, true, m_Pvd);
        
        PxSceneDesc sceneDesc(m_Factory->getTolerancesScale());
        sceneDesc.gravity = PhysicsUtils::FromVector3ToPxVec3(DEFAULT_GRAVITY_SCALE);
        m_Dispatcher = PxDefaultCpuDispatcherCreate(DEFAULT_NUMBER_CPU_THREADS);
        sceneDesc.cpuDispatcher	= m_Dispatcher;
        sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
        m_Collection = m_Factory->createScene(sceneDesc);

        if (s_IsDebugRenderEnabled)
        {
            m_Collection->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
            m_Collection->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
            m_Collection->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
        }
        
        if (s_IsPVDEnabled)
        {
            if (PxPvdSceneClient* pvdClient = m_Collection->getScenePvdClient())
            {
                pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
                pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
                pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
            }
        }
    }
    PhysicsBase::PhysicsBase()
    {
        InitPhysics();
    }
    
    bool PhysicsBase::Advance(float deltaTime)
    {
        s_Accumulator += deltaTime;
        
        if (s_Accumulator < s_StepSize) return false;
        
        s_Accumulator -= s_StepSize;
        s_PhysicsBase.m_Collection->simulate(s_StepSize);
        
        return true;
    }
    void PhysicsBase::StepPhysics(float deltaTime)
    {
        if (Advance(deltaTime))
        {
            s_PhysicsBase.m_Collection->fetchResults(true);
        }
    }
    void PhysicsBase::UpdateTransforms(entt::registry& registry)
    {
        const auto view = registry.view<PhysicsRigidbody, Transform>();
        for (const auto entity : view)
        {
            auto [transform, rigidbody] = view.get<Transform, PhysicsRigidbody>(entity);
            const PxTransform actorPose = rigidbody.GetRigidbody()->getGlobalPose();
            transform.SetWorldRotation(PhysicsUtils::FromPxQuatToQuaternion(actorPose.q));
            transform.SetWorldPosition(PhysicsUtils::FromPxVec3ToVector3(actorPose.p));
        }
    }
    void PhysicsBase::DrawDebugLines()
    {
        const PxRenderBuffer& rb = s_PhysicsBase.m_Collection->getRenderBuffer();
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
    void PhysicsBase::Process(entt::registry& m_Registry, float deltaTime)
    {
        StepPhysics(deltaTime);
        
        UpdateTransforms(m_Registry);
        
        if (s_IsDebugRenderEnabled)
        {
            DrawDebugLines();
        }
    }
    
    void PhysicsBase::CleanupPhysics()
    {
        PX_RELEASE(m_Collection)
        PX_RELEASE(m_Dispatcher)
        PX_RELEASE(m_Factory)
        if (m_Pvd)
        {
            PxPvdTransport* transport = m_Pvd->getTransport();
            PX_RELEASE(m_Pvd)
            PX_RELEASE(transport)
        }
        PX_RELEASE(m_Foundation)
    }
    PhysicsBase::~PhysicsBase()
    {
        CleanupPhysics();
    }

    PhysicsBase& PhysicsBase::GetInstance()
    {
        return s_PhysicsBase;
    }
    
    PxPhysics* PhysicsBase::GetFactory() const
    {
        return m_Factory;
    }

    PxScene* PhysicsBase::GetCollection() const
    {
        return m_Collection;
    }

    Vector3 PhysicsBase::GetGravity() const
    {
        const auto gravity = m_Collection->getGravity();
        return PhysicsUtils::FromPxVec3ToVector3(gravity);
    }

    void PhysicsBase::SetGravity(const Vector3 newGravity)
    {
        const auto gravity = PhysicsUtils::FromVector3ToPxVec3(newGravity);
        m_Collection->setGravity(gravity);
    }
}

