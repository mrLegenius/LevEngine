#pragma once
#include "physx/include/PxPhysicsAPI.h"

namespace LevEngine
{
    class PhysX
    {
    public:
        PhysX();
        ~PhysX();
    
    private:
        physx::PxDefaultAllocator      gAllocator;         // via this interface PhysX performs all allocations (need to initialize PhysX)
        physx::PxDefaultErrorCallback  gErrorCallback;     // via this interface PhysX logs all error messages (need to initialize PhysX)
        physx::PxTolerancesScale       gToleranceScale;    
        physx::PxFoundation*           gFoundation = NULL;
        physx::PxPhysics*              gPhysics    = NULL;
        physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
        physx::PxScene*                gScene      = NULL;
        physx::PxMaterial*             gMaterial   = NULL;
        physx::PxPvd*                  gPvd        = NULL;
    };
}
