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
        
        if (entity.HasComponent<CharacterController>() && hitEntity.HasComponent<Rigidbody>())
        {
            hitInfo.Entity = hitEntity;
            auto& controller = entity.GetComponent<CharacterController>();
            controller.m_CollisionHitBuffer.push_back(hitInfo);
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
        
        if (firstHitEntity.HasComponent<CharacterController>() && secondHitEntity.HasComponent<CharacterController>())
        {
            hitInfo.Entity = secondHitEntity;
            auto& firstController = firstHitEntity.GetComponent<CharacterController>();
            firstController.m_CollisionHitBuffer.push_back(hitInfo);

            hitInfo.Entity = firstHitEntity;
            auto& secondController = secondHitEntity.GetComponent<CharacterController>();
            secondController.m_CollisionHitBuffer.push_back(hitInfo);
        }
    }

    void CharacterControllerEventCallback::onObstacleHit(const physx::PxControllerObstacleHit& hit) {}
}