#pragma once
#include "Math/Vector3.h"
#include "physx/include/PxPhysicsAPI.h"
#include "Scene/Components/Transform/Transform.h"

//constexpr auto MAX_NUM_RIGIDBODY_SCENES    = 5;
constexpr auto MAX_NUM_RIGIDBODY_SHAPES    = 5;
constexpr auto MAX_NUM_RIGIDBODY_MATERIALS = 5;

namespace LevEngine
{
    using namespace physx;
    
    class PhysicsBase
    {
    public:
        PhysicsBase(const PhysicsBase&) = delete;
        PhysicsBase& operator=(const PhysicsBase&) = delete;
        
        static void Process(entt::registry& m_Registry, float deltaTime);

        [[nodiscard]] static PhysicsBase& GetInstance();
        
        [[nodiscard]] PxPhysics* GetPhysics() const;
        [[nodiscard]] PxScene* GetScene()     const;
        
        [[nodiscard]] Vector3 GetGravity() const;
        void SetGravity(const Vector3 gravity);

        // for debug
        inline static bool usePVD = false;
        inline static bool useDebugRender = true;
        
    private:
        PhysicsBase();
        ~PhysicsBase();

        void InitPhysics();
        void CleanupPhysics();

        static bool Advance(float deltaTime);
        static void StepPhysics(float deltaTime);
        static void UpdateTransforms(entt::registry& registry);
        static void DrawDebugLines();
        
        static PhysicsBase physx;
    
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
}
