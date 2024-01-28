#include "levpch.h"
#include "RigidbodyEventCallback.h"
#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/Components/Rigidbody.h"
#include "Physics/PhysicsUtils.h"
#include "Physics/Components/CharacterController.h"

namespace LevEngine
{
    physx::PxFilterFlags RigidbodyEventCallback::ContactReportFilterShader(
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
    
    void RigidbodyEventCallback::onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) {}
    void RigidbodyEventCallback::onWake(physx::PxActor**, physx::PxU32) {}
    void RigidbodyEventCallback::onSleep(physx::PxActor**, physx::PxU32) {}
    void RigidbodyEventCallback::onAdvance(const physx::PxRigidBody*const*, const physx::PxTransform*, const physx::PxU32) {}
    
    void RigidbodyEventCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs)
    {
        for (auto i = 0; i < nbPairs; i++)
        {
            const physx::PxTriggerPair& current = pairs[i];

            if (current.flags & physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER) continue;
            if (current.flags & physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER) continue;
            
            const auto triggerEntity = App::Get().GetPhysics().GetEntityByActor(current.triggerActor);
            auto& triggerRigidbody = triggerEntity.GetComponent<Rigidbody>();

            if (!triggerRigidbody.IsTriggerEnabled()) continue;
            
            if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                const auto otherEntity = App::Get().GetPhysics().GetEntityByActor(current.otherActor);
                triggerRigidbody.m_TriggerEnterBuffer.push_back(otherEntity);
            }
        
            if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                const auto otherEntity = App::Get().GetPhysics().GetEntityByActor(current.otherActor);
                triggerRigidbody.m_TriggerExitBuffer.push_back(otherEntity);
            }
        }
    }
    
    void RigidbodyEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) 
    {
        Vector<physx::PxContactPairPoint> contactPoints;
        
        for (auto i = 0; i < nbPairs; i++)
        {
            const physx::PxContactPair& current = pairs[i];

            if (current.flags & physx::PxContactPairFlag::eREMOVED_SHAPE_0) continue;
            if (current.flags & physx::PxContactPairFlag::eREMOVED_SHAPE_1) continue;
            
            auto collisionInfo = Collision {};
            if (const int contactCount = pairs[i].contactCount)
            {
                contactPoints.resize(contactCount);
                pairs[i].extractContacts(&contactPoints[0], contactCount);

                for (auto j = 0; j < contactCount; j++)
                {
                    collisionInfo.Points.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].position));
                    collisionInfo.Normals.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].normal));
                    collisionInfo.Impulses.push_back(PhysicsUtils::FromPxVec3ToVector3(contactPoints[j].impulse));
                    collisionInfo.Separations.push_back(contactPoints[j].separation);
                }
            }

            const auto firstEntity = App::Get().GetPhysics().GetEntityByActor(pairHeader.actors[0]);
            const auto secondEntity = App::Get().GetPhysics().GetEntityByActor(pairHeader.actors[1]);
            
            if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                if (firstEntity.HasComponent<Rigidbody>())
                {
                    auto& firstRigidbody = firstEntity.GetComponent<Rigidbody>();
                    collisionInfo.Entity = secondEntity;
                    firstRigidbody.m_CollisionEnterBuffer.push_back(collisionInfo);
                }
                
                if (secondEntity.HasComponent<Rigidbody>())
                {
                    auto& secondRigidbody = secondEntity.GetComponent<Rigidbody>();
                    collisionInfo.Entity = firstEntity;
                    secondRigidbody.m_CollisionEnterBuffer.push_back(collisionInfo);
                }
            }
            
            if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                if (firstEntity.HasComponent<Rigidbody>())
                {
                    auto& firstRigidbody = firstEntity.GetComponent<Rigidbody>();
                    collisionInfo.Entity = secondEntity;
                    firstRigidbody.m_CollisionExitBuffer.push_back(collisionInfo);
                }

                if (secondEntity.HasComponent<Rigidbody>())
                {
                    auto& secondRigidbody = secondEntity.GetComponent<Rigidbody>();
                    collisionInfo.Entity = firstEntity;
                    secondRigidbody.m_CollisionExitBuffer.push_back(collisionInfo);
                }
            }
        }
    }
}