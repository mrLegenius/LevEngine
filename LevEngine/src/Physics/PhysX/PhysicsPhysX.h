#pragma once
#include "Math/Vector3.h"
#include "physx/include/PxPhysicsAPI.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    using namespace physx;
    
    class PhysicsPhysX
    {
    public:
        PhysicsPhysX(const PhysicsPhysX&) = delete;
        PhysicsPhysX& operator=(const PhysicsPhysX&) = delete;
        
        static void Process(entt::registry& m_Registry, float deltaTime);

        [[nodiscard]] static PhysicsPhysX& GetInstance();
        
        [[nodiscard]] PxPhysics* GetPhysics() const;
        [[nodiscard]] PxScene*   GetScene()   const;
        [[nodiscard]] Vector3    GetGravity() const;
        
        void SetGravity(const Vector3 gravity);

        // for debug
        inline static bool usePVD = false;
        inline static bool useDebugRender = true;
        
    private:
        PhysicsPhysX();
        ~PhysicsPhysX();

        void InitPhysics();
        void CleanupPhysics();

        static bool Advance(float deltaTime);
        static void StepPhysics(float deltaTime);
        static void UpdateTransforms(entt::registry& registry);
        static void DrawDebugLines();
        
        static PhysicsPhysX physx;
    
        PxDefaultAllocator      gAllocator;
        PxDefaultErrorCallback  gErrorCallback;
        PxTolerancesScale       gToleranceScale;
        PxFoundation*           gFoundation      = NULL;
        PxPvd*                  gPvd             = NULL;
        PxPhysics*              gPhysics         = NULL;
        PxDefaultCpuDispatcher* gDispatcher      = NULL;
        PxScene*                gScene           = NULL;

        // for physics update
        inline static float mAccumulator = 0.0f;
        inline static float mStepSize    = 1.0f / 60.0f;
    };


    
    REGISTER_PARSE_TYPE(RigidbodyPhysX);

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
    
    class RigidbodyPhysX
    {
    public:
        RigidbodyPhysX()  = default;
        ~RigidbodyPhysX() = default;

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
