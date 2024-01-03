#pragma once
#include "Controller.h"
#include "characterkinematic/PxController.h"
#include "Scene/Entity.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(CharacterController);
    
    struct CharacterController
    {
        static void OnDestroy(entt::registry& registry, entt::entity entity);
        
        [[nodiscard]] bool IsInitialized() const;
        void Initialize(Entity entity);

        [[nodiscard]] Vector3 GetTransformScale() const;
        void SetTransformScale(Vector3 transformScale);

        [[nodiscard]] bool IsVisualizationEnabled() const;
        void EnableVisualization(bool flag);

        // Character Controller
        [[nodiscard]] float GetSlopeLimit() const;
        void SetSlopeLimit(float slopeLimit) const;
        [[nodiscard]] float GetStepOffset() const;
        void SetStepOffset(float stepOffset) const;
        [[nodiscard]] float GetContactOffset() const;
        void SetContactOffset(float contactOffset) const;
        [[nodiscard]] float GetMinimumMovementDistance() const;
        void SetMinimumMovementDistance(float minimumMovementDistance) const;
        [[nodiscard]] Controller::NonWalkableMode GetNonWalkableMode() const;
        void SetNonWalkableMode(const Controller::NonWalkableMode& nonWalkableMode) const;

        [[nodiscard]] Vector3 GetControllerCenter() const;
        void SetControllerCenter(Vector3 center) const;
        
        [[nodiscard]] float GetCapsuleControllerRadius() const;
        void SetCapsuleControllerRadius(float radius) const;
        [[nodiscard]] float GetCapsuleControllerHalfHeight() const;
        void SetCapsuleControllerHalfHeight(float halfHeight) const;
        [[nodiscard]] Controller::ClimbingMode GetCapsuleControllerClimbingMode() const;
        void SetCapsuleControllerClimbingMode(const Controller::ClimbingMode& climbingMode) const;
        
        [[nodiscard]] float GetStaticFriction() const;
        void SetStaticFriction(float staticFriction) const;
        [[nodiscard]] float GetDynamicFriction() const;
        void SetDynamicFriction(float dynamicFriction) const;
        [[nodiscard]] float GetRestitution() const;
        void SetRestitution(float restitution) const;

        void Move(Vector3 displacement, float elapsedTime) const;
        void Rotate();

        friend class PhysicsUpdate;

    private:
        [[nodiscard]] physx::PxController* GetController() const;
        [[nodiscard]] physx::PxRigidActor* GetActor() const;
        [[nodiscard]] physx::PxShape* GetCollider() const;
        [[nodiscard]] physx::PxMaterial* GetPhysicalMaterial() const;
        
        void AttachController(Entity entity);
        void DetachController() const;
        
        bool m_IsInitialized = false;

        bool m_IsVisualizationEnabled = true;
        
        physx::PxController* m_Controller = NULL;

        Vector3 m_TransformScale = Vector3::One;

        Ref<Controller> m_CharacterController { CreateRef<Controller>() };
    };
}
