#include "levpch.h"
#include "PhysicsUpdate.h"

#include "Physics.h"
#include "PhysicsUtils.h"
#include "Components/Rigidbody.h"

namespace LevEngine
{
    void PhysicsUpdate::ClearBuffers(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Transform, Rigidbody>();
        for (const auto entity : rigidbodyView)
        {
            auto [rigidbodyTransform, rigidbody] = rigidbodyView.get<Transform, Rigidbody>(entity);

            if (rigidbody.GetActor() == NULL) break;
            
            rigidbody.m_TriggerEnterBuffer.clear();
            rigidbody.m_TriggerExitBuffer.clear();
            rigidbody.m_CollisionEnterBuffer.clear();
            rigidbody.m_CollisionExitBuffer.clear();
        }
    }
    
    void PhysicsUpdate::UpdateTransforms(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Transform, Rigidbody>();
        for (const auto entity : rigidbodyView)
        {
            auto [transform, rigidbody] = rigidbodyView.get<Transform, Rigidbody>(entity);

            if (rigidbody.GetActor() == NULL) break;
            
            const physx::PxTransform actorPose = rigidbody.GetActor()->getGlobalPose();
            transform.SetWorldRotation(PhysicsUtils::FromPxQuatToQuaternion(actorPose.q));
            transform.SetWorldPosition(PhysicsUtils::FromPxVec3ToVector3(actorPose.p));
            rigidbody.SetTransformScale(transform.GetWorldScale());
        }
        
        const auto controllerView = registry.view<Transform, CharacterController>();
        for (const auto entity : controllerView)
        {
            auto [transform, controller] = controllerView.get<Transform, CharacterController>(entity);

            if (controller.GetController() == NULL) break;
            
            const physx::PxTransform actorPose = controller.GetActor()->getGlobalPose();
            transform.SetWorldPosition(PhysicsUtils::FromPxVec3ToVector3(actorPose.p));
            controller.SetTransformScale(transform.GetWorldScale());
        }
    }

    void PhysicsUpdate::UpdateConstantForces(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Rigidbody, ConstantForce>();
        for (const auto entity : rigidbodyView)
        {
            auto [rigidbody, constantForce] = rigidbodyView.get<Rigidbody, ConstantForce>(entity);

            if (rigidbody.GetActor() == NULL) break;
            
            rigidbody.AddForce(constantForce.GetForce(), Rigidbody::ForceMode::Force);
            rigidbody.AddTorque(constantForce.GetTorque(), Rigidbody::ForceMode::Force);
        }
    }

    void PhysicsUpdate::ApplyKinematicTargets(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Transform, Rigidbody>();
        for (const auto entity : rigidbodyView)
        {
            auto [rigidbodyTransform, rigidbody] = rigidbodyView.get<Transform, Rigidbody>(entity);

            if (rigidbody.GetActor() == NULL) break;

            if (!rigidbody.IsKinematicEnabled())
            {
                rigidbody.SetKinematicTargetRotation(rigidbodyTransform.GetWorldRotation());
                rigidbody.SetKinematicTargetPosition(rigidbodyTransform.GetWorldPosition());
            }
            
            rigidbody.ApplyKinematicTarget();
        }
    }
}