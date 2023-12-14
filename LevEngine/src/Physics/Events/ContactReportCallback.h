#pragma once
#include "PxSimulationEventCallback.h"

namespace LevEngine
{
    class ContactReportCallback: public physx::PxSimulationEventCallback
    {
    public:
        friend class Physics;
    private:
        static physx::PxFilterFlags ContactReportFilterShader(
            physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, 
            physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
            physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);
        
        void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override;
        void onWake(physx::PxActor**, physx::PxU32) override;
        void onSleep(physx::PxActor**, physx::PxU32) override;
        void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs) override;
        void onAdvance(const physx::PxRigidBody*const*, const physx::PxTransform*, physx::PxU32) override;
        void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
    };
}
