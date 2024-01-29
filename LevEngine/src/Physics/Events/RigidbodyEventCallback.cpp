#include "levpch.h"
#include "RigidbodyEventCallback.h"
#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsSettings.h"
#include "Physics/Components/Rigidbody.h"
#include "Physics/PhysicsUtils.h"
#include "Physics/Components/CharacterController.h"

namespace LevEngine
{
    physx::PxFilterFlags RigidbodyEventCallback::ContactReportFilterShader(
        physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
        physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
        physx::PxPairFlags& pairFlags, const void*, physx::PxU32 constantBlockSize
    )
    {
        if (!((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))) return physx::PxFilterFlag::eKILL;
        
        if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
        {
            pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
            
            return physx::PxFilterFlag::eDEFAULT;
        }
        
        pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT
                  | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
                  | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS
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

            const auto otherEntity = App::Get().GetPhysics().GetEntityByActor(current.otherActor);
            
            if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                triggerRigidbody.m_TriggerEnterBuffer.push_back(otherEntity);
                
                triggerRigidbody.m_TriggerStayBuffer.push_back(otherEntity);
            }
            
            if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                triggerRigidbody.m_TriggerExitBuffer.push_back(otherEntity);
                
                erase(triggerRigidbody.m_TriggerStayBuffer, otherEntity);
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

                collisionInfo.ContactCount = contactCount;
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

            if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
            {
                if (firstEntity.HasComponent<Rigidbody>())
                {
                    auto& firstRigidbody = firstEntity.GetComponent<Rigidbody>();
                    collisionInfo.Entity = secondEntity;
                    
                    erase_if(firstRigidbody.m_CollisionStayBuffer,
                        [&](const Collision& collision)
                        {
                            return collision.Entity == collisionInfo.Entity;
                        }
                    ); 
                    firstRigidbody.m_CollisionStayBuffer.push_back(collisionInfo);
                }

                if (secondEntity.HasComponent<Rigidbody>())
                {
                    auto& secondRigidbody = secondEntity.GetComponent<Rigidbody>();
                    collisionInfo.Entity = firstEntity;

                    erase_if(secondRigidbody.m_CollisionStayBuffer,
                        [&](const Collision& collision)
                        {
                            return collision.Entity == collisionInfo.Entity;
                        }
                    );
                    secondRigidbody.m_CollisionStayBuffer.push_back(collisionInfo);
                }
            }
            
            if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                if (firstEntity.HasComponent<Rigidbody>())
                {
                    auto& firstRigidbody = firstEntity.GetComponent<Rigidbody>();
                    collisionInfo.Entity = secondEntity;
                    
                    erase_if(firstRigidbody.m_CollisionStayBuffer,
                        [&](const Collision& collision)
                        {
                            return collision.Entity == collisionInfo.Entity;
                        }
                    );
                    firstRigidbody.m_CollisionExitBuffer.push_back(collisionInfo);
                }

                if (secondEntity.HasComponent<Rigidbody>())
                {
                    auto& secondRigidbody = secondEntity.GetComponent<Rigidbody>();
                    collisionInfo.Entity = firstEntity;

                    erase_if(secondRigidbody.m_CollisionStayBuffer,
                        [&](const Collision& collision)
                        {
                            return collision.Entity == collisionInfo.Entity;
                        }
                    );
                    secondRigidbody.m_CollisionExitBuffer.push_back(collisionInfo);
                }
            }
        }
    }
}