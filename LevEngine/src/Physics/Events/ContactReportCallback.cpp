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
                  | physx::PxPairFlag::eNOTIFY_TOUCH_LOST
                  | physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
        
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

            if (current.flags & physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER) break;
            if (current.flags & physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER) break;
            
            const auto& triggerEntity = entityMap.at(current.triggerActor);
            auto& triggerRigidbody = triggerEntity.GetComponent<Rigidbody>();

            if (!triggerRigidbody.IsTriggerEnabled()) break;
            
            if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
                && triggerRigidbody.m_IsTriggerEnterEnabled)
            {
                const auto& otherEntity = entityMap.at(current.otherActor);
                triggerRigidbody.m_TriggerEnterEntityBuffer.push_back(otherEntity);
                //Log::Debug("PAIR {0} ENTER ADDED TO {1}", i, triggerEntity.GetName());
            }
        
            if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST
                && triggerRigidbody.m_IsTriggerExitEnabled)
            {
                const auto& otherEntity = entityMap.at(current.otherActor);
                triggerRigidbody.m_TriggerExitEntityBuffer.push_back(otherEntity);
                //Log::Debug("PAIR {0} EXIT ADDED TO {1}", i, triggerEntity.GetName());
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
        Vector<Vector3> contactPositions;
        Vector<Vector3> contactNormals;
        Vector<Vector3> contactImpulses;
        Vector<float> contactSeparations;
        Vector<int> contactFirstColliderMaterialIndices;
        Vector<int> contactSecondColliderMaterialIndices;
        
        for (uint32_t i = 0; i < nbPairs; i++)
        {
            const physx::PxContactPair& current = pairs[i];

            if (current.flags & physx::PxContactPairFlag::eREMOVED_SHAPE_0) break;
            if (current.flags & physx::PxContactPairFlag::eREMOVED_SHAPE_1) break;
            
            // collision info struct holder
            auto collisionInfo = Collision {};
            // fill additional collision info
            if (const int contactCount = pairs[i].contactCount)
            {
                contactPoints.resize(contactCount);
                pairs[i].extractContacts(&contactPoints[0], contactCount);

                for (int j = 0; j < contactCount; j++)
                {
                    collisionInfo.ContactPositions.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].position));
                    collisionInfo.ContactNormals.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].normal));
                    collisionInfo.ContactImpulses.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].impulse));
                    collisionInfo.ContactSeparations.push_back(contactPoints[j].separation);
                    collisionInfo.ContactFirstColliderMaterialIndices.push_back(contactPoints[j].internalFaceIndex0);
                    collisionInfo.ContactSecondColliderMaterialIndices.push_back(contactPoints[j].internalFaceIndex1);
                }
            }
            
            const auto& firstCollisionEntity = entityMap.at(pairHeader.actors[0]);
            auto& firstCollisionRigidbody = firstCollisionEntity.GetComponent<Rigidbody>();
            
            const auto& secondCollisionEntity = entityMap.at(pairHeader.actors[1]);
            auto& secondCollisionRigidbody = secondCollisionEntity.GetComponent<Rigidbody>();
            
            if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                if (firstCollisionRigidbody.m_IsCollisionEnterEnabled)
                {
                    collisionInfo.ContactEntity = secondCollisionEntity;
                    firstCollisionRigidbody.m_CollisionEnterEntityBuffer.push_back(collisionInfo);
                    //Log::Debug("CONTACT FOUND");
                }
                if (secondCollisionRigidbody.m_IsCollisionEnterEnabled)
                {
                    collisionInfo.ContactEntity = firstCollisionEntity;
                    secondCollisionRigidbody.m_CollisionEnterEntityBuffer.push_back(collisionInfo);
                    //Log::Debug("CONTACT FOUND");
                }
            }
            
            if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                if (firstCollisionRigidbody.m_IsCollisionExitEnabled)
                {
                    collisionInfo.ContactEntity = secondCollisionEntity;
                    firstCollisionRigidbody.m_CollisionExitEntityBuffer.push_back(collisionInfo);
                    //Log::Debug("CONTACT LOST");
                }
                
                if (secondCollisionRigidbody.m_IsCollisionExitEnabled)
                {
                    collisionInfo.ContactEntity = firstCollisionEntity;
                    secondCollisionRigidbody.m_CollisionExitEntityBuffer.push_back(collisionInfo);
                    //Log::Debug("CONTACT LOST");
                }
            }
        }
    }
}
