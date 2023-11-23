#pragma once
#include "physx/include/PxPhysicsAPI.h"
#include "Scene/Entity.h"
#include "Math/Vector3.h"

namespace LevEngine
{
    class Physics
    {
    public:
        Physics(const Physics&) = delete;
        Physics& operator=(const Physics&) = delete;
        
        static void Process(entt::registry& registry, float deltaTime);

        [[nodiscard]] static Physics& GetInstance();
        
        [[nodiscard]] Vector3 GetGravity() const;
        void SetGravity(Vector3 gravity);

        friend struct Rigidbody;
        
    private:
        Physics();
        ~Physics();
        
        void Initialize();
        void Reset();

        static bool Advance(float deltaTime);
        static void StepPhysics(float deltaTime);
        static void UpdateTransforms(entt::registry& registry);
        static void DrawDebugLines();

        [[nodiscard]] physx::PxScene* GetScene() const;
        [[nodiscard]] physx::PxPhysics* GetPhysics() const;
        
        static Physics s_Physics;
        
        physx::PxDefaultAllocator m_Allocator;
        physx::PxDefaultErrorCallback m_ErrorCallback;
        physx::PxTolerancesScale m_ToleranceScale;
        
        physx::PxFoundation* m_Foundation = NULL;
        physx::PxPvd* m_Pvd = NULL;
        physx::PxDefaultCpuDispatcher* m_Dispatcher = NULL;
        physx::PxPhysics* m_Physics = NULL;
        physx::PxScene* m_Scene = NULL;
        
        // for debug
        inline static bool s_IsPVDEnabled = false;
        inline static bool s_IsDebugRenderEnabled = true;
        // for physics update
        inline static float s_Accumulator = 0.0f;
        inline static float s_StepSize = 1.0f / 60.0f;
    };
}
