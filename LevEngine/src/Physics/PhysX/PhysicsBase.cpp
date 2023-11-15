#include "levpch.h"
#include "PhysicsBase.h"

#include "PhysicsRigidbody.h"
#include "Renderer/DebugRender/DebugRender.h"
#include "Physics/PhysX/PhysicsUtils.h"

constexpr auto DEFAULT_PVD_HOST            = "127.0.0.1";
constexpr auto DEFAULT_PVD_PORT            = 5425;
constexpr auto DEFAULT_PVD_CONNECT_TIMEOUT = 10;
constexpr auto DEFAULT_NUMBER_CPU_THREADS  = 2;

constexpr Vector3 DEFAULT_GRAVITY_SCALE = {0.0f, -9.81f, 0.0f};

namespace LevEngine
{
    PhysicsBase PhysicsBase::physx;
    
    void PhysicsBase::InitPhysics()
    {
        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

        if (usePVD)
        {
            gPvd = PxCreatePvd(*gFoundation);
            PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(DEFAULT_PVD_HOST, DEFAULT_PVD_PORT, DEFAULT_PVD_CONNECT_TIMEOUT);
            gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);
        }
        
        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, gToleranceScale, true, gPvd);
        
        PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity = PhysicsUtils::FromVector3ToPxVec3(DEFAULT_GRAVITY_SCALE);
        gDispatcher = PxDefaultCpuDispatcherCreate(DEFAULT_NUMBER_CPU_THREADS);
        sceneDesc.cpuDispatcher	= gDispatcher;
        sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
        gScene = gPhysics->createScene(sceneDesc);

        if (useDebugRender)
        {
            gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
            gScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
            gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
        }
        
        if (usePVD)
        {
            PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
            if(pvdClient)
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
        mAccumulator += deltaTime;
        if (mAccumulator < mStepSize)
        {
            return false;
        }
        mAccumulator -= mStepSize;
        physx.gScene->simulate(mStepSize);
        return true;
    }
    void PhysicsBase::StepPhysics(float deltaTime)
    {
        if (Advance(deltaTime))
        {
            physx.gScene->fetchResults(true);
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
        const PxRenderBuffer& rb = physx.gScene->getRenderBuffer();
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
        if (useDebugRender)
        {
            DrawDebugLines();
        }
    }
    
    void PhysicsBase::CleanupPhysics()
    {
        PX_RELEASE(gScene)
        PX_RELEASE(gDispatcher)
        PX_RELEASE(gPhysics)
        if(gPvd)
        {
            PxPvdTransport* transport = gPvd->getTransport();
            PX_RELEASE(gPvd)
            PX_RELEASE(transport)
        }
        PX_RELEASE(gFoundation)
    }
    PhysicsBase::~PhysicsBase()
    {
        CleanupPhysics();
    }

    PhysicsBase& PhysicsBase::GetInstance()
    {
        return physx;
    }
    
    PxPhysics* PhysicsBase::GetPhysics() const
    {
        return gPhysics;
    }

    PxScene* PhysicsBase::GetScene() const
    {
        return gScene;
    }

    Vector3 PhysicsBase::GetGravity() const
    {
        const auto gravity = gScene->getGravity();
        return PhysicsUtils::FromPxVec3ToVector3(gravity);
    }

    void PhysicsBase::SetGravity(const Vector3 newGravity)
    {
        const auto gravity = PhysicsUtils::FromVector3ToPxVec3(newGravity);
        gScene->setGravity(gravity);
    }
}

