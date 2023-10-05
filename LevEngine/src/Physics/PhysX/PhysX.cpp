#include "levpch.h"
#include "PhysX.h"

namespace LevEngine
{
    PhysX::PhysX()
    {
        // init physx
        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
        if (!gFoundation) throw ("PxCreateFoundation failed!");
        
        gPvd = PxCreatePvd(*gFoundation);
        auto pvdHost = "127.0.0.1";
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(pvdHost, 5425, 10);
        gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
        
        gToleranceScale.length = 100;
        gToleranceScale.speed = 981;
        bool recordMemoryAllocations = true;
        physx::PxU32 version = PX_PHYSICS_VERSION;
        gPhysics = PxCreatePhysics(version, *gFoundation, gToleranceScale, recordMemoryAllocations, gPvd);
        if (!gPhysics) throw("PxCreatePhysics failed!");

        physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
        gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher = gDispatcher;
        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        gScene = gPhysics->createScene(sceneDesc);
        
        physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
        if(pvdClient)
        {
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }

        // create simulation
        gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
        physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*gPhysics, physx::PxPlane(0,1,0,1), *gMaterial);
        gScene->addActor(*groundPlane);

        // capsule
        physx::PxVec3 capsule_position = physx::PxVec3(0.0f, 5.0f, 0.0f);
        physx::PxReal capsule_radius = 1.0f;
        physx::PxReal capsule_halfHeight = 1.0f;
        physx::PxRigidDynamic* gCapsuleActor = gPhysics->createRigidDynamic(physx::PxTransform(capsule_position));
        physx::PxShape* gCapsuleShape = physx::PxRigidActorExt::createExclusiveShape(*gCapsuleActor, physx::PxCapsuleGeometry(capsule_radius, capsule_halfHeight), *gMaterial);
        physx::PxTransform capsuleRelativePose(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
        gCapsuleShape->setLocalPose(capsuleRelativePose);
        physx::PxReal capsuleDensity = 1.0f;
        physx::PxRigidBodyExt::updateMassAndInertia(*gCapsuleActor, capsuleDensity);
        gScene->addActor(*gCapsuleActor);
        gCapsuleShape->release();

        // box
        physx::PxVec3 box_position = physx::PxVec3(0.0f, 15.0f, 0.0f);
        physx::PxReal box_halfExtents = 1.0f;
        physx::PxRigidDynamic* gBoxActor = gPhysics->createRigidDynamic(physx::PxTransform(box_position));
        physx::PxShape* gBoxShape = physx::PxRigidActorExt::createExclusiveShape(*gBoxActor, physx::PxBoxGeometry(box_halfExtents, box_halfExtents, box_halfExtents), *gMaterial);
        physx::PxTransform boxRelativePose(physx::PxQuat(physx::PxPiDivFour, physx::PxVec3(0.0f, 0.0f, 1.0f)));
        gBoxShape->setLocalPose(boxRelativePose);
        physx::PxReal boxDensity = 1.0f;
        physx::PxRigidBodyExt::updateMassAndInertia(*gBoxActor, boxDensity);
        gScene->addActor(*gBoxActor);
        gBoxShape->release();
        
        // run simulation
        for (int i = 0; i < 150; i++)
        {
            gScene->simulate(1.0f/60.0f);
            gScene->fetchResults(true);
        }
    }

    PhysX::~PhysX()
    {
        PX_RELEASE(gScene)
        PX_RELEASE(gDispatcher)
        PX_RELEASE(gPhysics)
        if(gPvd)
        {
            physx::PxPvdTransport* transport = gPvd->getTransport();
            PX_RELEASE(gPvd)
            PX_RELEASE(transport)
        }
    }
}

