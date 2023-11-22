#pragma once
#include "Physics/Physics.h"
#include "Physics/PhysicsUtils.h"
#include "physx/include/PxPhysicsAPI.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(Rigidbody);

    using namespace physx;

    struct PhysicalMaterial
    {
        float StaticFriction = 0.5f;
        float DynamicFriction = 0.5f;
        float Restitution = 0.6f;
    };
    struct Collider
    {
        Vector3 OffsetPosition = Vector3::Zero;
        Vector3 OffsetRotation = Vector3::Zero;
        //TODO: CHANGE LOGIC FOR MULTIPLE MATERIAL ATTACHMENT
        Vector<PhysicalMaterial> MaterialCollection { PhysicalMaterial() };
    };
    struct Sphere : Collider
    {
        float Radius = 0.5f;
    };
    struct Capsule : Collider
    {
        float Radius = 0.5f;
        float HalfHeight = 0.5f;
    };
    struct Box : Collider
    {
        Vector3 HalfExtents = Vector3(0.5f, 0.5f, 0.5f);
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
    
    struct Rigidbody
    {
        static void OnDestroy(entt::registry& registry, entt::entity entity);

        Vector3 GetTransformScale() const;
        void SetTransformScale(const Vector3 transformScale);
        void ApplyTransformScale();
        
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
        void SetSphereRadius(const float);
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
        
        friend class Physics;
        
    private:
        [[nodiscard]] PxRigidActor* GetRigidbody() const;
        [[nodiscard]] PxShape* GetColliders() const;
        [[nodiscard]] PxMaterial* GetPhysicalMaterials(const PxShape* colliders) const;

        Vector3 m_TransformScale = Vector3::One;
        
        bool m_IsInitialized = false;
        
        PxRigidActor* m_Rigidbody = NULL;
        
        bool m_IsColliderVisualizationEnabled = false;
        RigidbodyType m_RigidbodyType = RigidbodyType::Dynamic;
        bool m_IsRigidbodyGravityEnabled = true;;
        ColliderType m_ColliderType = ColliderType::Box;
        //TODO: CHANGE LOGIC FOR MULTIPLE COLLIDER ATTACHMENT
        Vector<Ref<Collider>> m_ColliderCollection { CreateRef<Box>() };
    };
}