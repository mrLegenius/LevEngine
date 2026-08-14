#pragma once

#include "Kernel/Core.h"
#include "PhysicsUpdate.h"
#include "Components/FilterLayer.h"
#include "Components/RaycastHit.h"
#include "Events/CharacterControllerEventCallback.h"
#include "Events/RigidbodyEventCallback.h"
#include "Scene/Entity.h"

namespace LevEngine
{
    class LEV_API Physics
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
            FilterLayer layerMask = FilterLayer::Layer0
        ) const;
        
        [[nodiscard]] RaycastHit SphereCast(
            Vector3 origin,
            float radius,
            Vector3 direction,
            float maxDistance,
            FilterLayer layerMask = FilterLayer::Layer0
        ) const;
        
        [[nodiscard]] RaycastHit CapsuleCast(
            Vector3 origin,
            Quaternion orientation,
            float radius,
            float halfHeight,
            Vector3 direction,
            float maxDistance,
            FilterLayer layerMask = FilterLayer::Layer0
        ) const;
        
        [[nodiscard]] RaycastHit BoxCast(
            Vector3 origin,
            Quaternion orientation,
            Vector3 halfExtents,
            Vector3 direction,
            float maxDistance,
            FilterLayer layerMask = FilterLayer::Layer0
        ) const;
        
        // Cooks a triangle mesh and puts it in the scene as a static actor.
        //
        // Public, unlike the shape factories below, because the thing that needs it is not a
        // component: a planet's ground is generated and thrown away as the camera moves, so its
        // colliders come and go without an entity anywhere to hang them on. Positions are relative to
        // position, which is what keeps the cooked mesh's coordinates small -- see PlanetCollision.
        //
        // Returns null if the mesh is empty or the cooking fails. The caller owns the result and must
        // hand it back to RemoveStaticTriangleMesh.
        [[nodiscard]] physx::PxRigidStatic* CreateStaticTriangleMesh(
            const Vector<Vector3>& vertices,
            const Vector<uint32_t>& indices,
            Vector3 position,
            Quaternion rotation,
            FilterLayer layer = FilterLayer::Layer0);

        void RemoveStaticTriangleMesh(physx::PxRigidStatic* actor);

        void Process(entt::registry& registry, float deltaTime);
        
        friend struct Rigidbody;
        friend struct CharacterController;

        void ClearAccumulator();
        void ResetPhysicsScene();
        
    private:
        void Initialize();
        bool IsAdvanced(float deltaTime);
        bool StepPhysics(float deltaTime);
        void DrawDebugLines();

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