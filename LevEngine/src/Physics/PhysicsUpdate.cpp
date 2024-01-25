#include "levpch.h"
#include "PhysicsUtils.h"
#include "Physics.h"
#include "PhysicsUpdate.h"
#include "Components/CharacterController.h"
#include "Components/Rigidbody.h"
#include "Components/ConstantForce.h"
#include "Kernel/Application.h"

namespace LevEngine
{
    void PhysicsUpdate::ClearBuffers(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Transform, Rigidbody>();
        for (const auto entity : rigidbodyView)
        {
            auto [transform, rigidbody] = rigidbodyView.get<Transform, Rigidbody>(entity);

            if (rigidbody.GetActor() == nullptr) continue;

            /*
            if (rigidbody.IsTriggerEnabled())
            {
                const auto object = Entity(entt::handle(registry, entity));
                for (const auto element : rigidbody.m_TriggerStayBuffer)
                {
                    Log::Debug("{0} stays in {1}", element.GetName(), object.GetName());
                } 

                //Log::Debug("Enter: {0}", rigidbody.m_TriggerEnterBuffer.size());
                //Log::Debug("Stay: {0}", rigidbody.m_TriggerStayBuffer.size());
                //Log::Debug("Exit: {0}", rigidbody.m_TriggerExitBuffer.size());
            }
            */

            rigidbody.m_TriggerEnterBuffer.clear();
            rigidbody.m_TriggerExitBuffer.clear();

            Log::Debug("Enter: {0}", rigidbody.m_CollisionEnterBuffer.size());
            Log::Debug("Stay: {0}", rigidbody.m_CollisionStayBuffer.size());
            Log::Debug("Exit: {0}", rigidbody.m_CollisionExitBuffer.size());
            
            rigidbody.m_CollisionEnterBuffer.clear();
            rigidbody.m_CollisionExitBuffer.clear();
        }

        const auto controllerView = registry.view<Transform, CharacterController>();
        for (const auto entity : controllerView)
        {
            auto [transform, controller] = controllerView.get<Transform, CharacterController>(entity);

            if (controller.GetController() == nullptr) continue;
            
            controller.m_CollisionHitBuffer.clear();
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

    void PhysicsUpdate::UpdateControllerGroundFlag(entt::registry& registry)
    {
        const auto controllerView = registry.view<Transform, CharacterController>();
        for (const auto entity : controllerView)
        {
            auto [transform, controller] = controllerView.get<Transform, CharacterController>(entity);

            if (controller.GetController() == nullptr) continue;
            
            const auto position =
                PhysicsUtils::FromPxExtendedVec3ToVector3(controller.GetController()->getPosition());
            const auto rotation =
                Quaternion::CreateFromAxisAngle(Vector3::Forward, Math::ToRadians(90.0f));

            const auto controllerRadius = controller.GetRadius();
            const auto controllerHalfHeight = controller.GetHeight() / 2.0f;
            const auto hitResult =
                App::Get().GetPhysics().CapsuleCast(
                    position,
                    rotation,
                    controllerRadius,
                    controllerHalfHeight,
                    Vector3::Down,
                    0.1f
                );
            
            controller.SetGroundFlag(hitResult.IsSuccessful);
        }
    }

    void PhysicsUpdate::ApplyControllerGravity(entt::registry& registry, const float deltaTime)
    {
        const auto controllerView = registry.view<Transform, CharacterController>();
        for (const auto entity : controllerView)
        {
            auto [transform, controller] = controllerView.get<Transform, CharacterController>(entity);

            if (controller.GetController() == nullptr) continue;

            const auto gravity = App::Get().GetPhysics().GetGravity();
            
            const auto verticalVelocity =
                controller.GetVerticalVelocity() + gravity.y * controller.GetGravityScale() * deltaTime;
            controller.SetVerticalVelocity(verticalVelocity);

            const auto gravityDisplacement = verticalVelocity * deltaTime;
            controller.Move({0.0f, gravityDisplacement, 0.0f});
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
                rigidbody.SetKinematicTargetRotation(transform.GetWorldRotation()
                    * Quaternion::CreateFromAxisAngle(Vector3::Forward, Math::ToRadians(1.0f)));
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
