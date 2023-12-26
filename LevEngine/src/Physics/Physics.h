#pragma once

#include "physx/include/PxPhysicsAPI.h"
#include "Components/Rigidbody.h"
#include "Events/ContactReportCallback.h"
#include "Scene/Entity.h"
#include "PhysicsUpdate.h"
#include "Components\CharacterController.h"

namespace LevEngine
{
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
        
        // used to fix transfer scene information between game modes
        void ResetPhysicsScene();
        
        void Process(entt::registry& registry, float deltaTime);

        [[nodiscard]] Entity GetEntityByActor(physx::PxActor* actor) const;

        friend struct Rigidbody;
        friend struct CharacterController;
        
    private:
        void Initialize();
        void Reset();
        
        bool IsAdvanced(float deltaTime);
        bool StepPhysics(float deltaTime);
        void DrawDebugLines() const;

        [[nodiscard]] physx::PxRigidActor* CreateStaticActor(Entity entity);
        [[nodiscard]] physx::PxRigidActor* CreateDynamicActor(Entity entity);
        void RemoveActor(physx::PxActor* actor);

        [[nodiscard]] physx::PxMaterial* CreateMaterial(float staticFriction, float dynamicFriction, float restitution) const;
        
        [[nodiscard]] physx::PxShape* CreateSphere(float radius, const physx::PxMaterial* material) const;
        [[nodiscard]] physx::PxShape* CreateCapsule(float radius, float halfHeight, const physx::PxMaterial* material) const;
        [[nodiscard]] physx::PxShape* CreateBox(Vector3 halfExtents, const physx::PxMaterial* material) const;
        
        [[nodiscard]] physx::PxController* CreateBoxController(Entity entity, float halfHeight, float halfSideExtent, float halfForwardExtent, physx::PxMaterial* material);
        [[nodiscard]] physx::PxController* CreateCapsuleController(Entity entity, float radius, float height, CapsuleController::ClimbingMode climbingMode, physx::PxMaterial* material);
        void RemoveController(physx::PxController* controller);
        
        // used to optimize the filling of collision detection buffers
        UnorderedMap<physx::PxActor*, Entity> m_ActorEntityMap;

        // TODO: CHANGE PHYSICS UPDATE LOGIC
        // used to update step-dependent physics systems
        PhysicsUpdate m_PhysicsUpdate;

        // used to handle collision/trigger events
        ContactReportCallback m_ContactReportCallback;

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
        physx::PxControllerManager* m_ControllerManager = NULL; // single manager per scene
    };
}
