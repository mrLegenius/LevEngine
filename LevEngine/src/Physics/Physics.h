#pragma once

#include "physx/include/PxPhysicsAPI.h"

#include "Events/ContactReportCallback.h"
#include "Scene/Entity.h"

namespace LevEngine
{
    class PhysicsUpdate
    {
    public:
        friend class Physics;
    private:
        void UpdateTransforms(entt::registry& registry);
        void UpdateConstantForces(entt::registry& registry);
        void OneMoreStrangeSystem(entt::registry& registry);
        void HandleEvents(entt::registry& registry);
    };
    
    class Physics
    {
    public:
        static Scope<Physics> Create();
        
        explicit Physics();
        ~Physics();

        // some required set
        Physics(const Physics&) = delete;
        Physics& operator=(Physics const&) = delete;
        Physics(Physics&&) = delete;
        Physics& operator=(Physics&&) = delete;
        
        // used to fix accumulation transfer issue
        void ClearAccumulator();

        void ResetPhysicsScene();

        void Process(entt::registry& registry, float deltaTime);
        
        friend class ContactReportCallback;
        friend class RigidbodyInitSystem;
        friend struct Rigidbody;
        
    private:
        void Initialize();
        void Reset();
        
        bool IsAdvanced(float deltaTime);
        bool StepPhysics(float deltaTime);
        void DrawDebugLines() const;
        
        [[nodiscard]] physx::PxPhysics* GetPhysics() const;
        [[nodiscard]] physx::PxScene* GetScene() const;

        // TODO: CHANGE UPDATE LOGIC
        // used to update step-dependent physics systems
        PhysicsUpdate m_PhysicsUpdate;
        
        // used to receive collision detection
        ContactReportCallback m_ContactReportCallback;

        // used to optimize the filling of collision detection buffers
        UnorderedMap<physx::PxActor*, Entity> m_ActorEntityMap;
        
        // for debug render
        bool m_IsDebugRenderEnabled = true;
        // for physics update
        float m_Accumulator = 0.0f;
        float m_StepSize = 1.0f / 60.0f;
        // physx stuff
        physx::PxDefaultAllocator m_Allocator;
        physx::PxDefaultErrorCallback m_ErrorCallback;
        physx::PxFoundation* m_Foundation = NULL;
        physx::PxPhysics* m_Physics = NULL;
        physx::PxDefaultCpuDispatcher* m_Dispatcher = NULL;
        physx::PxScene* m_Scene = NULL;
    };
}
