#pragma once
#include "physx/include/PxPhysicsAPI.h"
#include "PhysicsUpdate.h"
#include "Components/FilterLayer.h"
#include "Components/RaycastHit.h"
#include "Events/CharacterControllerEventCallback.h"
#include "Events/RigidbodyEventCallback.h"
#include "Scene/Entity.h"

namespace LevEngine
{
    class Physics
    {
    public:
        
        static Scope<Physics> Create();
        
        explicit Physics();
        ~Physics();
        
        Physics(const Physics&) = delete;
        Physics& operator=(Physics const&) = delete;
        Physics(Physics&&) = delete;
        Physics& operator=(Physics&&) = delete;

        [[nodiscard]] Entity GetEntityByActor(const physx::PxActor* actor) const;
        
        [[nodiscard]] Vector3 GetGravity() const;
        
        [[nodiscard]] RaycastHit Raycast(
            Vector3 origin,
            Vector3 direction,
            float maxDistance,
            const FilterLayer& layerMask = FilterLayer::Layer0
        ) const;
        
        [[nodiscard]] RaycastHit SphereCast(
            Vector3 origin,
            float radius,
            Vector3 direction,
            float maxDistance,
            const FilterLayer& layerMask = FilterLayer::Layer0
        ) const;
        
        [[nodiscard]] RaycastHit CapsuleCast(
            Vector3 origin,
            Quaternion orientation,
            float radius,
            float halfHeight,
            Vector3 direction,
            float maxDistance,
            const FilterLayer& layerMask = FilterLayer::Layer0
        ) const;
        
        [[nodiscard]] RaycastHit BoxCast(
            Vector3 origin,
            Quaternion orientation,
            Vector3 halfExtents,
            Vector3 direction,
            float maxDistance,
            const FilterLayer& layerMask = FilterLayer::Layer0
        ) const;
        
        void Process(entt::registry& registry, float deltaTime);
        
        friend class Scene;
        friend struct Rigidbody;
        friend struct CharacterController;
        
    private:
        void Initialize();
        bool IsAdvanced(float deltaTime);
        bool StepPhysics(float deltaTime);
        void DrawDebugLines();
        void ClearAccumulator();
        void ResetPhysicsScene();
        void Reset();

        [[nodiscard]] physx::PxRigidActor* CreateStaticActor(Entity entity);
        [[nodiscard]] physx::PxRigidActor* CreateDynamicActor(Entity entity);
        void RemoveActor(physx::PxActor* actor);

        [[nodiscard]] physx::PxMaterial* CreatePhysicMaterial(float staticFriction, float dynamicFriction, float restitution) const;
        
        [[nodiscard]] physx::PxShape* CreateSphere(float radius) const;
        [[nodiscard]] physx::PxShape* CreateCapsule(float radius, float halfHeight) const;
        [[nodiscard]] physx::PxShape* CreateBox(Vector3 halfExtents) const;
        
        [[nodiscard]] physx::PxController* CreateCapsuleController(Entity entity, float radius, float height);
        void RemoveController(physx::PxController* controller);
        
        UnorderedMap<const physx::PxActor*, Entity> m_ActorEntityMap;

        // TODO: CHANGE PHYSICS UPDATE LOGIC
        PhysicsUpdate m_PhysicsUpdate;
        
        RigidbodyEventCallback m_RigidbodyEventCallback;
        CharacterControllerEventCallback m_CharacterControllerEventCallback;

        Vector3 m_GravityScale = {0.0f, -9.81f, 0.0f};
        
        bool m_IsDebugRenderEnabled = false;
        float m_Accumulator = 0.0f;
        float m_StepSize = 1.0f / 60.0f;
        physx::PxDefaultAllocator m_Allocator;
        physx::PxDefaultErrorCallback m_ErrorCallback;
        physx::PxFoundation* m_Foundation = nullptr;
        physx::PxPhysics* m_Physics = nullptr;
        physx::PxDefaultCpuDispatcher* m_Dispatcher = nullptr;
        physx::PxScene* m_Scene = nullptr;
        physx::PxControllerManager* m_ControllerManager = nullptr;
    };
}