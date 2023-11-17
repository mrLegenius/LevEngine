#pragma once
#include "physx/include/PxPhysicsAPI.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(PhysicsRigidbody);

    using namespace physx;
    
    using Collider = PxShape;
    using BaseActor = PxRigidActor;
    using StaticActor = PxRigidStatic;
    using DynamicActor = PxRigidDynamic;
    using PhysicalMaterial = PxMaterial;
    
    enum class RigidbodyType
    {
        Static,
        Dynamic
    };
    
    enum class ColliderType
    {
        Sphere,
        Capsule,
        Box
    };
    
    class PhysicsRigidbody
    {
    public:
        static void OnDestroy(entt::registry& registry, entt::entity entity);
        
        PhysicsRigidbody();
        
        void SetPose(const Transform& transform);
        
        void Initialize(Transform& transform);
        void Deinitialize();

        bool GetInitializationFlag() const;

        void CleanupRigidbody();

        [[nodiscard]] int GetColliderNumber() const;
        [[nodiscard]] Collider* GetColliders() const;
        [[nodiscard]] PhysicalMaterial* GetPhysicalMaterials(const Collider* colliders) const;

        [[nodiscard]] Vector3 GetColliderLocalPosition() const;
        [[nodiscard]] Vector3 GetColliderLocalRotation() const;
        void SetColliderLocalPosition(const Vector3 position);
        void SetColliderLocalRotation(const Vector3 rotation);
        
        [[nodiscard]] ColliderType GetColliderType() const;
        void AttachCollider(const ColliderType& colliderType);
        void DetachCollider();

        [[nodiscard]] float GetSphereColliderRadius() const;
        [[nodiscard]] float GetCapsuleColliderRadius() const;
        [[nodiscard]] float GetCapsuleColliderHalfHeight() const;
        [[nodiscard]] Vector3 GetBoxColliderHalfExtents() const;
        
        void SetSphereColliderRadius(const float radius);
        void SetCapsuleColliderRadius(const float radius);
        void SetCapsuleColliderHalfHeight(const float halfHeight);
        void SetBoxColliderHalfExtents(const Vector3 extends);
        
        [[nodiscard]] BaseActor* GetRigidbody() const;
        
        [[nodiscard]] RigidbodyType GetType() const;
        [[nodiscard]] bool GetGravityFlag() const;
        [[nodiscard]] bool GetColliderVisualizationFlag() const;
        [[nodiscard]] float GetStaticFriction() const;
        [[nodiscard]] float GetDynamicFriction() const;
        [[nodiscard]] float GetRestitution() const;

        void SetType(const RigidbodyType rigidbodyType);
        void SetGravityFlag(const bool flag);
        void SetColliderVisualizationFlag(const bool flag);
        void SetStaticFriction(const float staticFriction);
        void SetDynamicFriction(const float dynamicFriction);
        void SetRestitution(const float restitution);
        
    private:
        BaseActor* m_Actor = NULL;
        
        bool m_IsInitialized = false;

        // deserialized fields
        bool m_IsColliderVisualizationEnabled;
        RigidbodyType m_RigidbodyType;
        bool m_IsGravityEnabled;
        ColliderType m_ColliderType;
        float m_SphereRadius;
        float m_CapsuleRadius;
        float m_CapsuleHalfHeight;
        Vector3 m_BoxHalfExtents;
        Vector3 m_OffsetPosition;
        Vector3 m_OffsetRotation;
        float m_StaticFriction;
        float m_DynamicFriction;
        float m_Restitution;
    };
}