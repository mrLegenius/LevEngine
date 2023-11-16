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
        Capsule,
        Box
    };

    using namespace physx;
    
    class PhysicsRigidbody
    {
    public:
        static void OnDestroy(entt::registry& registry, entt::entity entity);
        
        PhysicsRigidbody();
        
        void SetRigidbodyInitialPose(const Transform& transform);
        void Init(Transform& transform);
        bool IsInitialized() const;
        void ResetInit();

        void CleanupRigidbody();

        [[nodiscard]] PxShape*    GetActorShapes()                         const;
        [[nodiscard]] PxMaterial* GetShapeMaterials(const PxShape* shapes) const;

        [[nodiscard]] Vector3 GetShapeLocalPosition() const;
        [[nodiscard]] Vector3 GetShapeLocalRotation() const;
        
        void SetShapeLocalPosition(const Vector3 position);
        void SetShapeLocalRotation(const Vector3 rotation);
        
        [[nodiscard]] ColliderType GetColliderType() const;
        void AttachCollider(const ColliderType& colliderType);
        void DetachCollider();

        [[nodiscard]] float   GetSphereColliderRadius()      const;
        [[nodiscard]] float   GetCapsuleColliderRadius()     const;
        [[nodiscard]] float   GetCapsuleColliderHalfHeight() const;
        [[nodiscard]] Vector3 GetBoxHalfExtends()            const;
        
        void SetSphereColliderRadius(const float radius);
        void SetCapsuleColliderRadius(const float radius);
        void SetCapsuleColliderHalfHeight(const float halfHeight);
        void SetBoxHalfExtends(const Vector3 extends);
        
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
        
        bool m_IsInited = false;
    };
}