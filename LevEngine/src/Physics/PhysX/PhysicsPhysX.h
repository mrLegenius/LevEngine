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
        void SetGravity(Vector3 gravity)   const { psScene->setGravity(PxVec3(gravity.x, gravity.y, gravity.z)); }

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
    class RigidbodyPhysX
    {
    public:
        RigidbodyPhysX() = delete;
        RigidbodyPhysX(const Transform& rbTransform);
        ~RigidbodyPhysX();

        void RemoveActor(); //
        void SetActorType(PxActorType::Enum requestedActorType); //
        
        void AttachShapeGeometry(PxGeometryType::Enum requestedGeometryType);
        void DetachShapeGeometry(PxU32 sequenceShapeNumber);

        [[nodiscard]] PxVec3 GetShapeGeometryParams(PxU32 sequenceShapeNumber);
        void SetShapeGeometryParam(PxU32 sequenceShapeNumber, PxU32 sequenceParamNumber, PxReal param);

        // Actor
        [[nodiscard]] PxRigidActor* GetActor() const { return rbActor; }
        [[nodiscard]] bool GetActorGravityStatus();
        [[nodiscard]] bool GetShapeGeometryVisualizationStatus();
        [[nodiscard]] PxReal GetActorMass();
        [[nodiscard]] PxVec3 GetActorLinearVelocity();
        [[nodiscard]] PxVec3 GetActorAngularVelocity();
        void SetActorGravityStatus(const bool status = true);
        void SetActorShapeGeometryVisualizationStatus(const bool status = true);
        void SetActorMass(PxReal mass);
        void SetActorLinearVelocity(PxVec3 linearVelocity);
        void SetActorAngularVelocity(PxVec3 angularVelocity);
        // Material
        [[nodiscard]] PxReal GetStaticFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber);
        [[nodiscard]] PxReal GetDynamicFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber);
        [[nodiscard]] PxReal GetRestitution(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber);
        void SetStaticFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal staticFriction = 0.0f);
        void SetDynamicFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal dynamicFriction = 0.0f);
        void SetRestitution(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal restitution = 0.0f);
        
    private:
        PxRigidActor* rbActor = nullptr;
    };
}
