#pragma once

#include "physx/include/PxPhysicsAPI.h"

namespace LevEngine
{
    class Physics
    {
    public:
        Physics();
        ~Physics();
        
        static Scope<Physics> Create();

        [[nodiscard]] physx::PxScene* GetScene() const;
        [[nodiscard]] physx::PxPhysics* GetPhysics() const;
        
        void Process(entt::registry& registry, float deltaTime);

        friend struct Rigidbody;
        
    private:
        void Initialize();
        void Reset();

        bool IsAdvanced(float deltaTime) const;
        void StepPhysics(float deltaTime);
        void UpdateTransforms(entt::registry& registry);
        void DrawDebugLines();
        
        physx::PxDefaultAllocator m_Allocator;
        physx::PxDefaultErrorCallback m_ErrorCallback;
        physx::PxTolerancesScale m_ToleranceScale;
        
        physx::PxFoundation* m_Foundation = NULL;
        physx::PxPvd* m_Pvd = NULL;
        physx::PxDefaultCpuDispatcher* m_Dispatcher = NULL;
        physx::PxPhysics* m_Physics = NULL;
        physx::PxScene* m_Scene = NULL;

        Vector3 m_Gravity = Vector3(0.0f, -9.81f, 0.0f);
        
        // for debug
        inline static bool s_IsPVDEnabled = false;
        inline static bool s_IsDebugRenderEnabled = true;
        // for physics update
        inline static float s_Accumulator = 0.0f;
        inline static float s_StepSize = 1.0f / 60.0f;
    };
}
