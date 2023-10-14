#pragma once
#include "Math/Vector3.h"
#include "physx/include/PxPhysicsAPI.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    using namespace physx;

    /// --- PhysX Part --- ///
    class PhysicsPhysX
    {
    public:
        PhysicsPhysX(const PhysicsPhysX&) = delete;
        PhysicsPhysX& operator=(const PhysicsPhysX&) = delete;
        
        static bool Advance(float deltaTime);
        static void StepPhysics(float deltaTime);
        static void UpdateTransforms(entt::registry& registry);
        static void Process(entt::registry& m_Registry, float deltaTime);
        static void DrawDebugLines();

        static PhysicsPhysX& GetInstance() { return physx; };
        
        [[nodiscard]] PxPhysics* GetPhysics() const { return psPhysics; }
        [[nodiscard]] PxScene*   GetScene()   const { return psScene;   }
        
        [[nodiscard]] Vector3 GetGravity() const { return Vector3(psScene->getGravity().x, psScene->getGravity().y, psScene->getGravity().z); }
        
        void SetGravity(Vector3 gravity) { psScene->setGravity(PxVec3(gravity.x, gravity.y, gravity.z)); }

        inline static bool bDebug = true;
        
    private:
        PhysicsPhysX();
        ~PhysicsPhysX();

        void InitPhysics();
        void CleanupPhysics();
        
        static PhysicsPhysX physx;
    
        PxDefaultAllocator      psAllocator;
        PxDefaultErrorCallback  psErrorCallback;
        PxTolerancesScale       psToleranceScale;
        PxFoundation*           psFoundation = nullptr;
        PxPvd*                  psPvd        = nullptr;
        PxPhysics*              psPhysics    = nullptr;
        PxDefaultCpuDispatcher* psDispatcher = nullptr;
        PxScene*                psScene      = nullptr;

        inline static float mAccumulator = 0.0f;
        inline static float mStepSize    = 1.0f / 120.0f;
    };

    
    
    /// --- Rigidbody Part --- ///
    // -- material -- //
    class RigidbodyMaterial
    {
    public:
        explicit RigidbodyMaterial(float staticFriction = 0.5f, float dynamicFriction = 0.5f, float restitution = 0.6f)
            : staticFriction(staticFriction), dynamicFriction(dynamicFriction), restitution(restitution)
        {
            rbMaterial = PhysicsPhysX::GetInstance().GetPhysics()->createMaterial(staticFriction, dynamicFriction, restitution);
        }
        ~RigidbodyMaterial()
        {
            PX_RELEASE(rbMaterial)
        };

        [[nodiscard]] PxMaterial* GetMaterial() const { return rbMaterial; }
        
        [[nodiscard]] float GetRestitution()     const { return restitution;     }
        [[nodiscard]] float GetStaticFriction()  const { return staticFriction;  }
        [[nodiscard]] float GetDynamicFriction() const { return dynamicFriction; }
        
        void SetRestitution(float newRestitution)         { restitution     = newRestitution;     rbMaterial->setRestitution(restitution);         }
        void SetStaticFriction(float newStaticFriction)   { staticFriction  = newStaticFriction;  rbMaterial->setStaticFriction(staticFriction);   }
        void SetDynamicFriction(float newDynamicFriction) { dynamicFriction = newDynamicFriction; rbMaterial->setDynamicFriction(dynamicFriction); }
        
    private:
        float staticFriction;
        float dynamicFriction;
        float restitution;  
        PxMaterial* rbMaterial = nullptr;
    };
    
    class RigidbodyPhysX
    {
    public:
        RigidbodyPhysX() = delete;
        RigidbodyPhysX(const Transform& rbTransform);
        ~RigidbodyPhysX();

        void CleanupActor();
        
        void SetActorType(PxActorType::Enum requestedActorType);
        
        void AttachShapeGeometry(PxGeometryType::Enum requestedGeometryType);
        void DetachShapeGeometryType(PxU32 sequenceShapeNumber);
        
        void SetShapeGeometryParams(PxU32 sequenceShapeNumber, PxReal param1 = 0.0f, PxReal param2 = 0.0f, PxReal param3 = 0.0f);

        // Actor Flags
        void SetActorGravityStatus(const bool status = true);
        void SetActorShapeGeometryVisualizationStatus(const bool status = true);
        // Actor Params
        void SetActorMass(PxReal mass);
        void SetActorLinearVelocity(PxVec3 linearVelocity);
        void SetActorAngularVelocity(PxVec3 angularVelocity);

        // Material Params
        void SetStaticFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal staticFriction = 0.0f);
        void SetDynamicFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal dynamicFriction = 0.0f);
        void SetRestitution(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal restitution = 0.0f);
        
        [[nodiscard]] PxRigidActor* GetActor() const { return rbActor; }
        
    private:
        PxRigidActor* rbActor = nullptr;
    };
}
