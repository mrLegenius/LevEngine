#pragma once
#include "physx/include/PxPhysicsAPI.h"
#include "Scene/Entity.h"
#include "Math/Vector3.h"

namespace LevEngine
{
    using namespace physx;
    
    class Physics
    {
    public:
        Physics(const Physics&) = delete;
        Physics& operator=(const Physics&) = delete;
        
        static void Process(entt::registry& registry, float deltaTime);

        [[nodiscard]] static Physics& GetInstance();
        
        [[nodiscard]] Vector3 GetGravity() const;
        void SetGravity(const Vector3 gravity);

        friend struct Rigidbody;
        
    private:
        Physics();
        ~Physics();
        
        void Initialize();
        void Deinitialize();

        static bool Advance(float deltaTime);
        static void StepPhysics(float deltaTime);
        static void UpdateTransforms(entt::registry& registry);
        static void DrawDebugLines();

        [[nodiscard]] PxScene* GetScene() const;
        [[nodiscard]] PxPhysics* GetPhysics() const;
        
        static Physics s_Physics;
        
        PxDefaultAllocator m_Allocator;
        PxDefaultErrorCallback m_ErrorCallback;
        PxTolerancesScale m_ToleranceScale;
        
        PxFoundation* m_Foundation = NULL;
        PxPvd* m_Pvd = NULL;
        PxDefaultCpuDispatcher* m_Dispatcher = NULL;
        PxPhysics* m_Physics = NULL;
        PxScene* m_Scene = NULL;
        
        // for debug
        inline static bool s_IsPVDEnabled = false;
        inline static bool s_IsDebugRenderEnabled = true;
        // for physics update
        inline static float s_Accumulator = 0.0f;
        inline static float s_StepSize = 1.0f / 60.0f;
    };
}
