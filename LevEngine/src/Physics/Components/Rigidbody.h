﻿#pragma once
#include "physx/include/PxPhysicsAPI.h"
#include "Collider.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    struct Transform;
    REGISTER_PARSE_TYPE(Rigidbody);
    
    struct Rigidbody
    {
        enum class Type
        {
            Static,
            Dynamic
        };
        
        static void OnDestroy(entt::registry& registry, entt::entity entity);

        Vector3 GetTransformScale() const;
        void SetTransformScale(Vector3 transformScale);
        
        bool IsInitialized() const;
        void Initialize(const Transform& transform);
        void SetRigidbodyPose(const Transform& transform);
        
        [[nodiscard]] Type GetRigidbodyType() const;
        void SetRigidbodyType(const Type& rigidbodyType);
        void AttachRigidbody(const Type& rigidbodyType);
        void DetachRigidbody();
        [[nodiscard]] bool IsGravityEnabled() const;
        void EnableGravity(bool flag);

        [[nodiscard]] float GetMass() const;
        void SetMass(float value);
        [[nodiscard]] Vector3 GetCenterOfMass() const;
        void SetCenterOfMass(Vector3 value);
        [[nodiscard]] Vector3 GetInertiaTensor() const;
        void SetInertiaTensor(Vector3 value);
        
        [[nodiscard]] float GetLinearDamping() const;
        void SetLinearDamping(float value);
        [[nodiscard]] float GetAngularDamping() const;
        void SetAngularDamping(float value);
        
        [[nodiscard]] bool IsPosAxisXLocked() const;
        void LockPosAxisX(bool flag);
        [[nodiscard]] bool IsPosAxisYLocked() const;
        void LockPosAxisY(bool flag);
        [[nodiscard]] bool IsPosAxisZLocked() const;
        void LockPosAxisZ(bool flag);
        [[nodiscard]] bool IsRotAxisXLocked() const;
        void LockRotAxisX(bool flag);
        [[nodiscard]] bool IsRotAxisYLocked() const;
        void LockRotAxisY(bool flag);
        [[nodiscard]] bool IsRotAxisZLocked() const;
        void LockRotAxisZ(bool flag);

        [[nodiscard]] Vector3 GetAppliedForce() const;
        void ApplyForce(Vector3 value);
        [[nodiscard]] Vector3 GetAppliedTorque() const;
        void ApplyTorque(Vector3 value);
        
        [[nodiscard]] Collider::Type GetColliderType() const;
        void SetColliderType(const Collider::Type& colliderType);
        void AttachCollider(const Collider::Type& colliderType);
        void DetachCollider();
        [[nodiscard]] int GetColliderCount() const;
        [[nodiscard]] bool IsVisualizationEnabled() const;
        void EnableVisualization(bool flag);
        
        [[nodiscard]] Vector3 GetColliderOffsetPosition() const;
        void SetColliderOffsetPosition(Vector3 position);
        [[nodiscard]] Vector3 GetColliderOffsetRotation() const;
        void SetColliderOffsetRotation(Vector3 rotation);

        [[nodiscard]] float GetSphereRadius() const;
        void SetSphereRadius(float radius);
        [[nodiscard]] float GetCapsuleRadius() const;
        void SetCapsuleRadius(float radius);
        [[nodiscard]] float GetCapsuleHalfHeight() const;
        void SetCapsuleHalfHeight(float halfHeight);
        [[nodiscard]] Vector3 GetBoxHalfExtents() const;
        void SetBoxHalfExtents(Vector3 halfExtents);
        
        [[nodiscard]] float GetStaticFriction() const;
        void SetStaticFriction(float staticFriction);
        [[nodiscard]] float GetDynamicFriction() const;
        void SetDynamicFriction(float dynamicFriction);
        [[nodiscard]] float GetRestitution() const;
        void SetRestitution(float restitution);
        
        friend class Physics;
        
    private:
        [[nodiscard]] physx::PxRigidActor* GetRigidbody() const;
        [[nodiscard]] physx::PxShape* GetColliders() const;
        [[nodiscard]] physx::PxMaterial* GetPhysicalMaterials(const physx::PxShape* colliders) const;
        
        physx::PxRigidActor* m_Actor = nullptr;

        Type m_Type = Type::Dynamic;

        Vector3 m_TransformScale = Vector3::One;
        
        bool m_IsInitialized = false;
        bool m_IsVisualizationEnabled = false;
        bool m_IsGravityEnabled = true;
        
        //TODO: CHANGE LOGIC FOR MULTIPLE COLLIDER ATTACHMENT
        Vector<Ref<Collider>> m_ColliderCollection { CreateRef<Box>() };
        
        float m_Mass = 1.0f;
        float m_LinearDamping = 0.0f;
        float m_AngularDamping = 0.05f;
        Vector3 m_CenterOfMass = Vector3::Zero;
        Vector3 m_InertiaTensor = Vector3::One;
        
        bool m_IsPosAxisXLocked = false;
        bool m_IsPosAxisYLocked = false;
        bool m_IsPosAxisZLocked = false;

        bool m_IsRotAxisXLocked = false;
        bool m_IsRotAxisYLocked = false;
        bool m_IsRotAxisZLocked = false;

        Vector3 m_AppliedForce = Vector3::Zero;
        Vector3 m_AppliedTorque = Vector3::Zero;
    };
}
