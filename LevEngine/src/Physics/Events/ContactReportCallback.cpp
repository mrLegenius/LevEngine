#include "levpch.h"
#include "ContactReportCallback.h"

#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/Components/Rigidbody.h"

namespace LevEngine
{
    physx::PxFilterFlags ContactReportCallback::ContactReportFilterShader(
        physx::PxFilterObjectAttributes attributes0, physx::PxFilterData,
        physx::PxFilterObjectAttributes attributes1, physx::PxFilterData,
        physx::PxPairFlags& pairFlags, const void*, physx::PxU32)
    {
        if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
        {
            pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
            return physx::PxFilterFlag::eDEFAULT;
        }
        
        pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT
                  | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
                  | physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
        
        return physx::PxFilterFlag::eDEFAULT;
    }
    
    void ContactReportCallback::onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32)
    {
        Log::Debug("onConstraintBreak");
    }
    
    void ContactReportCallback::onWake(physx::PxActor**, physx::PxU32)
    {
        Log::Debug("onWake");
    }
    
    void ContactReportCallback::onSleep(physx::PxActor**, physx::PxU32)
    {
        Log::Debug("onSleep");
    }
    
    void ContactReportCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs)
    {
        const auto& entityMap = App::Get().GetPhysics().m_ActorEntityMap;
        
        while (nbPairs--)
        {
            const physx::PxTriggerPair& current = *pairs++;
            
            const auto& triggerEntity = entityMap.at(current.triggerActor);
            auto& triggerRigidbody = triggerEntity.GetComponent<Rigidbody>();
            const auto& otherEntity = entityMap.at(current.otherActor);
            
            if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                if (triggerRigidbody.IsTriggerEnabled())
                {
                    triggerRigidbody.m_TriggerEnterEntityBuffer.push_back(otherEntity);
                }
            }
            
            if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                if (triggerRigidbody.IsTriggerEnabled())
                {
                    triggerRigidbody.m_TriggerExitEntityBuffer.push_back(otherEntity);
                }
            }
        }
    }
    
    void ContactReportCallback::onAdvance(const physx::PxRigidBody*const*, const physx::PxTransform*, const physx::PxU32)
    {
        Log::Debug("onAdvance");
    }
    
    void ContactReportCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) 
    {
        const auto& entityMap = App::Get().GetPhysics().m_ActorEntityMap;
        
        while (nbPairs--)
        {
            const physx::PxContactPair& current = *pairs++;
            
            const auto& firstCollisionEntity = entityMap.at(pairHeader.actors[0]);
            auto& firstCollisionRigidbody = firstCollisionEntity.GetComponent<Rigidbody>();
            
            const auto& secondCollisionEntity = entityMap.at(pairHeader.actors[1]);
            auto& secondCollisionRigidbody = secondCollisionEntity.GetComponent<Rigidbody>();
            
            if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                if (firstCollisionRigidbody.IsContactEnabled())
                {
                    firstCollisionRigidbody.m_CollisionEnterEntityBuffer.push_back(secondCollisionEntity);
                }
                if (secondCollisionRigidbody.IsContactEnabled())
                {
                    secondCollisionRigidbody.m_CollisionEnterEntityBuffer.push_back(firstCollisionEntity);
                }
            }

            if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                if (firstCollisionRigidbody.IsContactEnabled())
                {
                    firstCollisionRigidbody.m_CollisionExitEntityBuffer.push_back(secondCollisionEntity);
                }
                if (secondCollisionRigidbody.IsContactEnabled())
                {
                    secondCollisionRigidbody.m_CollisionExitEntityBuffer.push_back(firstCollisionEntity);
                }
            }
        }
    }
}