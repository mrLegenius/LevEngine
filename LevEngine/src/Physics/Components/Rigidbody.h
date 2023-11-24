#pragma once
#include "physx/include/PxPhysicsAPI.h"
#include "Scene/Components/Transform/Transform.h"
#include "Collider.h"

namespace LevEngine
{
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
        
        physx::PxRigidActor* m_Actor = NULL;

        Type m_Type = Type::Dynamic;

        Vector3 m_TransformScale = Vector3::One;
        
        bool m_IsInitialized = false;
        bool m_IsVisualizationEnabled = false;
        bool m_IsGravityEnabled = true;
        //TODO: CHANGE LOGIC FOR MULTIPLE COLLIDER ATTACHMENT
        Vector<Ref<Collider>> m_ColliderCollection { CreateRef<Box>() };
    };
}