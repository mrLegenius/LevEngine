#include "levpch.h"
#include "ContactReportCallback.h"

#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/Components/Rigidbody.h"
#include "Physics/Support/PhysicsUtils.h"

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

        for (uint32_t i = 0; i < nbPairs; i++)
        {
            const physx::PxTriggerPair& current = pairs[i];

            const auto& triggerEntity = entityMap.at(current.triggerActor);
            auto& triggerRigidbody = triggerEntity.GetComponent<Rigidbody>();
            
            if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                if (triggerRigidbody.IsTriggerEnabled() && triggerRigidbody.m_IsTriggerEnterEnabled)
                {
                    const auto& otherEntity = entityMap.at(current.otherActor);
                    const auto& collision = Collision { otherEntity };
                    triggerRigidbody.m_TriggerEnterEntityBuffer.push_back(collision);
                }
            }
            
            if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                if (triggerRigidbody.IsTriggerEnabled() && triggerRigidbody.m_IsTriggerExitEnabled)
                {
                    const auto& otherEntity = entityMap.at(current.otherActor);
                    const auto& collision = Collision { otherEntity };
                    triggerRigidbody.m_TriggerExitEntityBuffer.push_back(collision);
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

        Vector<physx::PxContactPairPoint> contactPoints;
        
        for (uint32_t i = 0; i < nbPairs; i++)
        {
            const physx::PxContactPair& current = pairs[i];
            //const physx::PxU32 contactCount = current.contactCount;
            
            const auto& firstCollisionEntity = entityMap.at(pairHeader.actors[0]);
            auto& firstCollisionRigidbody = firstCollisionEntity.GetComponent<Rigidbody>();
            
            const auto& secondCollisionEntity = entityMap.at(pairHeader.actors[1]);
            auto& secondCollisionRigidbody = secondCollisionEntity.GetComponent<Rigidbody>();
            
            if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                /*
                current.extractContacts(&contactPoints[0], contactCount);
                for(physx::PxU32 j = 0; j < contactCount; j++)
                {
                    m_ContactPositions.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].position));
                    m_ContactNormals.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].normal));
                    m_ContactImpulses.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].impulse));
                    m_ContactSeparations.push_back(contactPoints[j].separation);
                    m_ContactFirstColliderMaterialIndices.push_back(contactPoints[j].internalFaceIndex0);
                    m_ContactSecondColliderMaterialIndices.push_back(contactPoints[j].internalFaceIndex1);
                }
                */
                
                if (firstCollisionRigidbody.m_IsCollisionEnterEnabled)
                {
                    const auto& collision = Collision { secondCollisionEntity };
                    firstCollisionRigidbody.m_CollisionEnterEntityBuffer.push_back(collision);
                }
                
                if (secondCollisionRigidbody.m_IsCollisionEnterEnabled)
                {
                    const auto& collision = Collision { firstCollisionEntity };
                    secondCollisionRigidbody.m_CollisionEnterEntityBuffer.push_back(collision);
                }
            }
            
            if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                /*
                current.extractContacts(&contactPoints[0], contactCount);
                for(physx::PxU32 j = 0; j < contactCount; j++)
                {
                    m_ContactPositions.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].position));
                    m_ContactNormals.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].normal));
                    m_ContactImpulses.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].impulse));
                    m_ContactSeparations.push_back(contactPoints[j].separation);
                    m_ContactFirstColliderMaterialIndices.push_back(contactPoints[j].internalFaceIndex0);
                    m_ContactSecondColliderMaterialIndices.push_back(contactPoints[j].internalFaceIndex1);
                }
                */
                
                if (firstCollisionRigidbody.m_IsCollisionExitEnabled)
                {
                    const auto& collision = Collision { secondCollisionEntity };
                    firstCollisionRigidbody.m_CollisionExitEntityBuffer.push_back(collision);
                }
                
                if (secondCollisionRigidbody.m_IsCollisionExitEnabled)
                {
                    const auto& collision = Collision { firstCollisionEntity };
                    secondCollisionRigidbody.m_CollisionExitEntityBuffer.push_back(collision);
                }
            }
        }
    }
}
