#pragma once
#include "physx/include/PxPhysicsAPI.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(PhysicsRigidbody);

    enum class RigidbodyType
    {
        Static,
        Dynamic
    };
    
    enum class ColliderType
    {
        Sphere,
        Plane,
        Capsule,
        Box
    };

    using namespace physx;
    
    class PhysicsRigidbody
    {
    public:
        PhysicsRigidbody()  = default;
        ~PhysicsRigidbody() = default;

        static void OnComponentConstruct(entt::registry& registry, entt::entity entity);
        static void OnComponentDestroy(entt::registry& registry, entt::entity entity);
        
        void SetRigidbodyInitialPose(const Transform& transform);
        
        void CleanupRigidbody();

        [[nodiscard]] ColliderType GetColliderType() const;
        void AttachCollider(const ColliderType& colliderType);
        void DetachCollider();

        float GetSphereColliderRadius()      const;
        float GetCapsuleColliderRadius()     const;
        float GetCapsuleColliderHalfHeight() const;
        float GetBoxColliderHalfExtendX()    const;
        float GetBoxColliderHalfExtendY()    const;
        float GetBoxColliderHalfExtendZ()    const;
        
        void SetSphereColliderRadius(float radius);
        void SetCapsuleColliderRadius(float radius);
        void SetCapsuleColliderHalfHeight(float halfHeight);
        void SetBoxColliderHalfExtendX(float halfExtendX);
        void SetBoxColliderHalfExtendY(float halfExtendY);
        void SetBoxColliderHalfExtendZ(float halfExtendZ);
        
        [[nodiscard]] PxRigidActor* GetRigidbody() const;
        
        [[nodiscard]] RigidbodyType GetRigidbodyType()             const;
        [[nodiscard]] bool          GetRigidbodyGravityFlag()      const;
        [[nodiscard]] bool          GetColliderVisualizationFlag() const;
        [[nodiscard]] float         GetStaticFriction()            const;
        [[nodiscard]] float         GetDynamicFriction()           const;
        [[nodiscard]] float         GetRestitution()               const;

        void SetRigidbodyType(const RigidbodyType rigidbodyType);
        void SetRigidbodyGravityFlag(const bool flag);
        void SetColliderVisualizationFlag(const bool flag);
        void SetStaticFriction(const float staticFriction);
        void SetDynamicFriction(const float dynamicFriction);
        void SetRestitution(const float restitution);
        
    private:
        PxRigidActor* rbActor = NULL;
    };
}