#include "levpch.h"
#include "PhysicsUtils.h"
#include "Physics.h"
#include "PhysicsUpdate.h"
#include "Components/Rigidbody.h"
#include "Components/ConstantForce.h"

namespace LevEngine
{
    void PhysicsUpdate::ClearBuffers(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Transform, Rigidbody>();
        for (const auto entity : rigidbodyView)
        {
            auto [transform, rigidbody] = rigidbodyView.get<Transform, Rigidbody>(entity);

            if (rigidbody.GetActor() == nullptr) continue;
            
            rigidbody.m_TriggerEnterBuffer.clear();
            rigidbody.m_TriggerExitBuffer.clear();
            rigidbody.m_CollisionEnterBuffer.clear();
            rigidbody.m_CollisionExitBuffer.clear();
        }

        const auto controllerView = registry.view<Transform, CharacterController>();
        for (const auto entity : controllerView)
        {
            auto [transform, controller] = controllerView.get<Transform, CharacterController>(entity);

            if (controller.GetController() == nullptr) continue;
            
            controller.m_CollisionEnterBuffer.clear();
            controller.m_CollisionExitBuffer.clear();
        }
    }
    
    void PhysicsUpdate::UpdateTransforms(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Transform, Rigidbody>();
        for (const auto entity : rigidbodyView)
        {
            auto [transform, rigidbody] = rigidbodyView.get<Transform, Rigidbody>(entity);

            if (rigidbody.GetActor() == nullptr) continue;
            
            const physx::PxTransform actorPose = rigidbody.GetActor()->getGlobalPose();
            transform.SetWorldRotation(PhysicsUtils::FromPxQuatToQuaternion(actorPose.q));
            transform.SetWorldPosition(PhysicsUtils::FromPxVec3ToVector3(actorPose.p));
            rigidbody.SetTransformScale(transform.GetWorldScale());
        }
        
        const auto controllerView = registry.view<Transform, CharacterController>();
        for (const auto entity : controllerView)
        {
            auto [transform, controller] = controllerView.get<Transform, CharacterController>(entity);

            if (controller.GetController() == nullptr) continue;
            
            const auto transformPosition =
                PhysicsUtils::FromPxExtendedVec3ToVector3(controller.GetController()->getPosition()) - controller.GetCenterOffset();
            transform.SetWorldPosition(transformPosition);
            controller.SetTransformScale(transform.GetWorldScale());
        }
    }

    void PhysicsUpdate::UpdateConstantForces(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Rigidbody, ConstantForce>();
        for (const auto entity : rigidbodyView)
        {
            auto [rigidbody, force] = rigidbodyView.get<Rigidbody, ConstantForce>(entity);

            if (rigidbody.GetActor() == nullptr) continue;
            
            rigidbody.AddForce(force.GetForce(), Rigidbody::ForceMode::Force);
            rigidbody.AddTorque(force.GetTorque(), Rigidbody::ForceMode::Force);
        }
    }

    void PhysicsUpdate::ApplyKinematicTargets(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Transform, Rigidbody>();
        for (const auto entity : rigidbodyView)
        {
            auto [transform, rigidbody] = rigidbodyView.get<Transform, Rigidbody>(entity);

            if (rigidbody.GetActor() == nullptr) continue;

            if (rigidbody.IsKinematicEnabled())
            {
                rigidbody.ApplyKinematicTarget();
            }
            else
            {
                rigidbody.SetKinematicTargetPosition(transform.GetWorldPosition());
                rigidbody.SetKinematicTargetRotation(transform.GetWorldRotation());
            }
        }
    }
}