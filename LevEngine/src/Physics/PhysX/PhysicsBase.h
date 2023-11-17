#pragma once
#include "physx/include/PxPhysicsAPI.h"
#include "Scene/Components/Transform/Transform.h"

constexpr auto MAX_NUM_RIGIDBODY_SHAPES = 5;
constexpr auto MAX_NUM_RIGIDBODY_MATERIALS = 5;

namespace LevEngine
{
    using namespace physx;

    using Collection = PxScene;
    using Factory = PxPhysics;
    
    class PhysicsBase
    {
    public:
        PhysicsBase(const PhysicsBase&) = delete;
        PhysicsBase& operator=(const PhysicsBase&) = delete;
        
        static void Process(entt::registry& m_Registry, float deltaTime);

        [[nodiscard]] static PhysicsBase& GetInstance();
        
        [[nodiscard]] Factory* GetFactory() const;
        [[nodiscard]] Collection* GetCollection() const;
        
        [[nodiscard]] Vector3 GetGravity() const;
        void SetGravity(const Vector3 gravity);
        
    private:
        PhysicsBase();
        ~PhysicsBase();

        void InitPhysics();
        void CleanupPhysics();

        static bool Advance(float deltaTime);
        static void StepPhysics(float deltaTime);
        static void UpdateTransforms(entt::registry& registry);
        static void DrawDebugLines();
        
        static PhysicsBase s_PhysicsBase;
    
        PxDefaultAllocator m_Allocator;
        PxDefaultErrorCallback m_ErrorCallback;
        PxTolerancesScale m_ToleranceScale;
        
        PxFoundation* m_Foundation = NULL;
        PxPvd* m_Pvd = NULL;
        PxPhysics* m_Factory = NULL;
        PxDefaultCpuDispatcher* m_Dispatcher = NULL;
        PxScene* m_Collection = NULL;
        
        // for debug
        inline static bool s_IsPVDEnabled = false;
        inline static bool s_IsDebugRenderEnabled = true;
        // for physics update
        inline static float s_Accumulator = 0.0f;
        inline static float s_StepSize = 1.0f / 60.0f;
    };
}
