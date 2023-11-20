#pragma once
#include "PhysicsRigidbody.h"
#include "PhysicsUtils.h"
#include "physx/include/PxPhysicsAPI.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(PhysicsRigidbody);

    using namespace physx;
    
    using PCollider = PxShape;
    using PPhysicalMaterial = PxMaterial;
    using PBaseRigidbody = PxRigidActor;
    using PStaticRigidbody = PxRigidStatic;
    using PDynamicRigidbody = PxRigidDynamic;

    struct PhysicalMaterial
    {
        float StaticFriction;
        float DynamicFriction;
        float Restitution;
    };
    struct Collider
    {
        Vector3 OffsetPosition;
        Vector3 OffsetRotation;
        //TODO: CHANGE LOGIC FOR MULTIPLE MATERIAL ATTACHMENT
        Vector<PhysicalMaterial> MaterialCollection;
    };
    struct Sphere : Collider
    {
        float Radius;
    };
    struct Capsule : Collider
    {
        float Radius;
        float HalfHeight;
    };
    struct Box : Collider
    {
        Vector3 HalfExtents;
    };
    
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

        [[nodiscard]] PBaseRigidbody* GetRigidbody() const;
        
        bool GetInitializationFlag() const;
        void Initialize(const Transform& transform);
        void SetRigidbodyPose(const Transform& transform);
        
        [[nodiscard]] RigidbodyType GetRigidbodyType() const;
        void SetRigidbodyType(const RigidbodyType& rigidbodyType);
        void AttachRigidbody(const RigidbodyType& rigidbodyType);
        void DetachRigidbody();
        [[nodiscard]] bool GetRigidbodyGravityFlag() const;
        void SetRigidbodyGravityFlag(const bool flag);
        
        [[nodiscard]] ColliderType GetColliderType() const;
        void SetColliderType(const ColliderType& colliderType);
        void AttachCollider(const ColliderType& colliderType);
        void DetachCollider();
        [[nodiscard]] int GetColliderNumber() const;
        [[nodiscard]] bool GetColliderVisualizationFlag() const;
        void SetColliderVisualizationFlag(const bool flag);
        
        [[nodiscard]] Vector3 GetColliderOffsetPosition() const;
        void SetColliderOffsetPosition(const Vector3 position);
        [[nodiscard]] Vector3 GetColliderOffsetRotation() const;
        void SetColliderOffsetRotation(const Vector3 rotation);

        [[nodiscard]] float GetSphereRadius() const;
        void SetSphereRadius(const float radius);
        [[nodiscard]] float GetCapsuleRadius() const;
        void SetCapsuleRadius(const float radius);
        [[nodiscard]] float GetCapsuleHalfHeight() const;
        void SetCapsuleHalfHeight(const float halfHeight);
        [[nodiscard]] Vector3 GetBoxHalfExtents() const;
        void SetBoxHalfExtents(const Vector3 halfExtents);
        
        [[nodiscard]] float GetStaticFriction() const;
        void SetStaticFriction(const float staticFriction);
        [[nodiscard]] float GetDynamicFriction() const;
        void SetDynamicFriction(const float dynamicFriction);
        [[nodiscard]] float GetRestitution() const;
        void SetRestitution(const float restitution);
        
    private:
        [[nodiscard]] PCollider* GetColliders() const;
        [[nodiscard]] PPhysicalMaterial* GetPhysicalMaterials(const PCollider* colliders) const;
        
        bool m_IsInitialized = false;
        
        PBaseRigidbody* m_Rigidbody = NULL;
        
        bool m_IsColliderVisualizationEnabled;
        RigidbodyType m_RigidbodyType;
        bool m_IsRigidbodyGravityEnabled;
        ColliderType m_ColliderType;
        //TODO: CHANGE LOGIC FOR MULTIPLE COLLIDER ATTACHMENT
        Vector<Ref<Collider>> m_ColliderCollection;
    };
}