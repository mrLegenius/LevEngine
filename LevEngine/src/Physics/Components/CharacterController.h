#pragma once
#include "characterkinematic/PxController.h"
#include "Scene/Components/TypeParseTraits.h"
#include "Controller.h"
#include "ControllerColliderHit.h"
#include "Kernel/Time/Timestep.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(CharacterController);
    
    struct CharacterController
    {
        // Don't call this method (only for internal use)
        static void OnDestroy(entt::registry& registry, entt::entity entity);

        [[nodiscard]] FilterLayer GetLayer() const;
        void SetLayer(const FilterLayer& layer) const;
        
        [[nodiscard]] float GetSlopeLimit() const;
        void SetSlopeLimit(float slopeLimit) const;
        [[nodiscard]] float GetStepOffset() const;
        void SetStepOffset(float stepOffset) const;
        [[nodiscard]] float GetSkinWidth() const;
        void SetSkinWidth(float contactOffset) const;
        [[nodiscard]] float GetMinMoveDistance() const;
        void SetMinMoveDistance(float minimumMovementDistance) const;

        [[nodiscard]] Vector3 GetCenterOffset() const;
        void SetCenterOffset(Vector3 center) const;
        
        [[nodiscard]] float GetRadius() const;
        void SetRadius(float radius) const;
        [[nodiscard]] float GetHeight() const;
        void SetHeight(float height) const;
        
        [[nodiscard]] float GetDynamicFriction() const;
        void SetDynamicFriction(float dynamicFriction) const;
        [[nodiscard]] float GetStaticFriction() const;
        void SetStaticFriction(float staticFriction) const;
        [[nodiscard]] float GetBounciness() const;
        void SetBounciness(float bounciness) const;
        [[nodiscard]] PhysicMaterial::CombineMode GetFrictionCombineMode() const;
        void SetFrictionCombineMode(const PhysicMaterial::CombineMode& frictionCombineMode) const;
        [[nodiscard]] PhysicMaterial::CombineMode GetBounceCombineMode() const;
        void SetBounceCombineMode(const PhysicMaterial::CombineMode& bounceCombineMode) const;

        [[nodiscard]] Controller::NonWalkableMode GetNonWalkableMode() const;
        void SetNonWalkableMode(const Controller::NonWalkableMode& nonWalkableMode) const;
        [[nodiscard]] Controller::ClimbingMode GetClimbingMode() const;
        void SetClimbingMode(const Controller::ClimbingMode& climbingMode) const;
        
        [[nodiscard]] float GetGravityScale() const;
        void SetGravityScale(float gravityScale) const;

        void Move(Vector3 displacement);
        void MoveTo(Vector3 position);

        [[nodiscard]] bool IsGrounded() const;
        void Jump(float jumpHeight, float deltaTime);
        
        [[nodiscard]] const Vector<ControllerColliderHit>& GetCollisionHitBuffer() const;

        Timestep m_LastMoveTime;

        friend class CharacterControllerInitSystem;
        friend class PhysicsUpdate;

        friend class CharacterControllerEventCallback;

    private:
        [[nodiscard]] physx::PxController* GetController() const;
        [[nodiscard]] physx::PxRigidActor* GetActor() const;
        [[nodiscard]] physx::PxShape* GetCollider() const;
        [[nodiscard]] physx::PxMaterial* GetPhysicalMaterial() const;
        
        [[nodiscard]] bool IsInitialized() const;
        void Initialize(Entity entity);
        
        [[nodiscard]] Vector3 GetTransformScale() const;
        void SetTransformScale(Vector3 scale);
        
        [[nodiscard]] bool IsVisualizationEnabled() const;
        void EnableVisualization(bool flag);
        
        void AttachController(Entity entity);
        void DetachController() const;

        void SetGroundFlag(bool flag) const;

        [[nodiscard]] float GetVerticalVelocity() const;
        void SetVerticalVelocity(float verticalVelocity) const;

        physx::PxController* m_Controller = nullptr;
        
        bool m_IsInitialized = false;
        Vector3 m_TransformScale = Vector3::One;
        
        bool m_IsVisualizationEnabled = false;
        
        Ref<Controller> m_CharacterController { CreateRef<Controller>() };

        Vector<ControllerColliderHit> m_CollisionHitBuffer;
    };
}
