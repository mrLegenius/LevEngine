#include "levpch.h"
#include "CharacterControllerEventCallback.h"
#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsUtils.h"
#include "Physics/Components/Rigidbody.h"
#include "Physics/Components/CharacterController.h"
#include "Physics/Components/ControllerColliderHit.h"

namespace LevEngine
{
    void CharacterControllerEventCallback::onShapeHit(const physx::PxControllerShapeHit& hit)
    {
        auto hitInfo = ControllerColliderHit {};
        hitInfo.Point = PhysicsUtils::FromPxExtendedVec3ToVector3(hit.worldPos);
        hitInfo.Normal = PhysicsUtils::FromPxVec3ToVector3(hit.worldNormal);
        hitInfo.MoveDirection = PhysicsUtils::FromPxVec3ToVector3(hit.dir);
        hitInfo.MoveLength = hit.length;

        const auto entity = App::Get().GetPhysics().GetEntityByActor(hit.controller->getActor());
        const auto hitEntity = App::Get().GetPhysics().GetEntityByActor(hit.actor);
        
        if (entity.HasComponent<CharacterController>())
        {
            if (hitEntity.HasComponent<Rigidbody>())
            {
                hitInfo.Entity = hitEntity;
                auto& controller = entity.GetComponent<CharacterController>();
                controller.m_CollisionHitBuffer.push_back(hitInfo);
            }
        }
    }

    void CharacterControllerEventCallback::onControllerHit(const physx::PxControllersHit& hit)
    {
        auto hitInfo = ControllerColliderHit {};
        hitInfo.Point = PhysicsUtils::FromPxExtendedVec3ToVector3(hit.worldPos);
        hitInfo.Normal = PhysicsUtils::FromPxVec3ToVector3(hit.worldNormal);
        hitInfo.MoveDirection = PhysicsUtils::FromPxVec3ToVector3(hit.dir);
        hitInfo.MoveLength = hit.length;
        
        const auto firstHitEntity = App::Get().GetPhysics().GetEntityByActor(hit.controller->getActor());
        const auto secondHitEntity = App::Get().GetPhysics().GetEntityByActor(hit.other->getActor());
        
        if (firstHitEntity.HasComponent<CharacterController>())
        {
            if (secondHitEntity.HasComponent<CharacterController>())
            {
                hitInfo.Entity = secondHitEntity;
                auto& controller = firstHitEntity.GetComponent<CharacterController>();
                controller.m_CollisionHitBuffer.push_back(hitInfo);
            }
        }
        
        if (secondHitEntity.HasComponent<CharacterController>())
        {
            if (firstHitEntity.HasComponent<CharacterController>())
            {
                hitInfo.Entity = firstHitEntity;
                auto& controller = secondHitEntity.GetComponent<CharacterController>();
                controller.m_CollisionHitBuffer.push_back(hitInfo);
            }
        }
    }

    void CharacterControllerEventCallback::onObstacleHit(const physx::PxControllerObstacleHit& hit) {}
}