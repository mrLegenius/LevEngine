﻿#pragma once

#include "physx/include/PxPhysicsAPI.h"

namespace LevEngine
{
    class Physics
    {
    public:
        Physics();
        ~Physics();

        static Scope<Physics> Create();

        void Process(entt::registry& registry, float deltaTime);

        void ClearAccumulator();

        [[nodiscard]] physx::PxScene* GetScene() const;
        [[nodiscard]] physx::PxPhysics* GetPhysics() const;
        
    private:
        void Initialize();
        void Reset();
        
        bool IsAdvanced(float deltaTime);
        bool StepPhysics(float deltaTime);
        void DrawDebugLines() const;
        
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
        bool m_IsPVDEnabled = false;
        bool m_IsDebugRenderEnabled = true;
        // for physics update
        float m_Accumulator = 0.0f;
        float m_StepSize = 1.0f / 60.0f;
    };
}