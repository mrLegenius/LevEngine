﻿#include "levpch.h"
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

            if (rigidbody.GetActor() == NULL) return;

            if (!rigidbody.m_TriggerEnterBuffer.empty())
            {
                rigidbody.m_TriggerEnterBuffer.clear();
            }
            if (!rigidbody.m_TriggerExitBuffer.empty())
            {
                rigidbody.m_TriggerExitBuffer.clear();
            }
            if (!rigidbody.m_CollisionEnterBuffer.empty())
            {
                rigidbody.m_CollisionEnterBuffer.clear();
            }
            if (!rigidbody.m_CollisionExitBuffer.empty())
            {
                rigidbody.m_CollisionExitBuffer.clear();
            }
        }
    }
    
    void PhysicsUpdate::UpdateTransforms(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Transform, Rigidbody>();
        for (const auto entity : rigidbodyView)
        {
            auto [rigidbodyTransform, rigidbody] = rigidbodyView.get<Transform, Rigidbody>(entity);

            if (rigidbody.GetActor() == NULL) return;
            
            const physx::PxTransform actorPose = rigidbody.GetActor()->getGlobalPose();
            rigidbodyTransform.SetWorldRotation(PhysicsUtils::FromPxQuatToQuaternion(actorPose.q));
            rigidbodyTransform.SetWorldPosition(PhysicsUtils::FromPxVec3ToVector3(actorPose.p));
        
            rigidbody.SetTransformScale(rigidbodyTransform.GetWorldScale());
        }
    }

    void PhysicsUpdate::UpdateConstantForces(entt::registry& registry)
    {
        const auto rigidbodyView = registry.view<Rigidbody, ConstantForce>();
        for (const auto entity : rigidbodyView)
        {
            auto [rigidbody, constantForce] = rigidbodyView.get<Rigidbody, ConstantForce>(entity);

            if (rigidbody.GetActor() == NULL) return;
            
            rigidbody.AddForce(constantForce.GetForce(), Rigidbody::ForceMode::Force);
            rigidbody.AddTorque(constantForce.GetTorque(), Rigidbody::ForceMode::Force);
        }
    }
}