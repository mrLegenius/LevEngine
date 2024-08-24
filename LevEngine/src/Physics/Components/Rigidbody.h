#pragma once
#include "Scene/Components/TypeParseTraits.h"
#include "Collider.h"
#include "PhysicMaterial.h"
#include "Collision.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(Rigidbody);

    struct Rigidbody
    {
        enum class Type
        {
            Static,
            Dynamic
        };

        enum class ForceMode
        {
            // Add a continuous force to the rigidbody, using its mass
            Force,
            // Add an instant force impulse to the rigidbody, using its mass
            Impulse,
            // Add a continuous acceleration to the rigidbody, ignoring its mass
            Acceleration,
            // Add an instant velocity change to the rigidbody, ignoring its mass
            VelocityChange
        };

        static void OnConstruct(Entity entity);
        // Don't call this method (only for internal use)
        static void OnDestroy(Entity entity);
        
        [[nodiscard]] FilterLayer GetLayer() const;
        void SetLayer(FilterLayer layer) const;
        
        [[nodiscard]] Type GetRigidbodyType() const;
        void SetRigidbodyType(const Type& type);
        
        [[nodiscard]] float GetMass() const;
        void SetMass(float mass);
        [[nodiscard]] float GetLinearDamping() const;
        void SetLinearDamping(float linearDamping);
        [[nodiscard]] float GetAngularDamping() const;
        void SetAngularDamping(float angularDamping);
        [[nodiscard]] Vector3 GetCenterOfMass() const;
        void SetCenterOfMass(Vector3 centerOfMass);
        [[nodiscard]] Vector3 GetInertiaTensor() const;
        void SetInertiaTensor(Vector3 inertiaTensor);

        [[nodiscard]] bool IsGravityEnabled() const;
        void EnableGravity(bool flag);
        
        [[nodiscard]] bool IsKinematicEnabled() const;
        void EnableKinematic(bool flag);
        [[nodiscard]] Vector3 GetKinematicTargetPosition() const;
        void SetKinematicTargetPosition(Vector3 position);
        [[nodiscard]] Quaternion GetKinematicTargetRotation() const;
        void SetKinematicTargetRotation(Quaternion rotation);
        
        [[nodiscard]] bool IsPosLocked() const;
        void LockPos(bool flag);
        [[nodiscard]] bool IsRotLocked() const;
        void LockRot(bool flag);
        [[nodiscard]] bool IsPosAxisXLocked() const;
        void LockPosAxisX(bool flag);
        [[nodiscard]] bool IsPosAxisYLocked() const;
        void LockPosAxisY(bool flag);
        [[nodiscard]] bool IsPosAxisZLocked() const;
        void LockPosAxisZ(bool flag);
        [[nodiscard]] bool IsRotAxisXLocked() const;
        void LockRotAxisX(bool flag);
        [[nodiscard]] bool IsRotAxisYLocked() const;
        void LockRotAxisY(bool flag);
        [[nodiscard]] bool IsRotAxisZLocked() const;
        void LockRotAxisZ(bool flag);
        
        [[nodiscard]] bool IsTriggerEnabled() const;
        void EnableTrigger(bool flag);
        
        [[nodiscard]] Collider::Type GetColliderType() const;
        void SetColliderType(const Collider::Type& type);
        [[nodiscard]] int GetColliderCount() const;
        
        [[nodiscard]] Vector3 GetColliderOffsetPosition() const;
        void SetColliderOffsetPosition(Vector3 position);
        [[nodiscard]] Vector3 GetColliderOffsetRotation() const;
        void SetColliderOffsetRotation(Vector3 rotation);
        
        [[nodiscard]] float GetSphereRadius() const;
        void SetSphereRadius(float radius);
        [[nodiscard]] float GetCapsuleRadius() const;
        void SetCapsuleRadius(float radius);
        [[nodiscard]] float GetCapsuleHalfHeight() const;
        void SetCapsuleHalfHeight(float halfHeight);
        [[nodiscard]] Vector3 GetBoxHalfExtents() const;
        void SetBoxHalfExtents(Vector3 halfExtents);
        
        [[nodiscard]] float GetDynamicFriction() const;
        void SetDynamicFriction(float dynamicFriction);
        [[nodiscard]] float GetStaticFriction() const;
        void SetStaticFriction(float staticFriction);
        [[nodiscard]] float GetBounciness() const;
        void SetBounciness(float bounciness);
        [[nodiscard]] PhysicMaterial::CombineMode GetFrictionCombineMode() const;
        void SetFrictionCombineMode(const PhysicMaterial::CombineMode& frictionCombineMode) const;
        [[nodiscard]] PhysicMaterial::CombineMode GetBounceCombineMode() const;
        void SetBounceCombineMode(const PhysicMaterial::CombineMode& bounceCombineMode) const;

        void AddForce(Vector3 force, ForceMode mode = ForceMode::Force) const;
        void AddTorque(Vector3 torque, ForceMode mode = ForceMode::Force) const;

        void Teleport(Vector3 position);
        
        [[nodiscard]] const Vector<Entity>& GetTriggerEnterBuffer() const;
        [[nodiscard]] const Vector<Entity>& GetTriggerStayBuffer() const;
        [[nodiscard]] const Vector<Entity>& GetTriggerExitBuffer() const;
        [[nodiscard]] const Vector<Collision>& GetCollisionEnterBuffer() const;
        [[nodiscard]] const Vector<Collision>& GetCollisionStayBuffer() const;
        [[nodiscard]] const Vector<Collision>& GetCollisionExitBuffer() const;

        friend class PhysicsUpdate;
        
        friend class RigidbodyEventCallback;
        
    private:
        void Initialize(Entity entity);
        
        [[nodiscard]] physx::PxRigidActor* GetActor() const;
        [[nodiscard]] physx::PxShape* GetCollider() const;
        [[nodiscard]] physx::PxMaterial* GetPhysicalMaterial() const;

        [[nodiscard]] Vector3 GetTransformScale() const;
        void SetTransformScale(Vector3 transformScale);

        [[nodiscard]] bool IsVisualizationEnabled() const;
        void EnableVisualization(bool flag);

        void AttachRigidbody(Entity entity);
        void DetachRigidbody();
        
        void AttachCollider();
        void DetachCollider();

        void ApplyKinematicTarget() const;

        physx::PxRigidActor* m_Actor = nullptr;
        
        Vector3 m_TransformScale = Vector3::One;
        
        bool m_IsVisualizationEnabled = false;

        Type m_Type = Type::Dynamic;

        float m_Mass = 1.0f;
        float m_LinearDamping = 0.0f;
        float m_AngularDamping = 0.05f;
        Vector3 m_CenterOfMass = Vector3::Zero;
        Vector3 m_InertiaTensor = Vector3::One;
        
        bool m_IsGravityEnabled = true;
        
        bool m_IsKinematicEnabled = false;
        Vector3 m_KinematicTargetPosition = Vector3::Zero;
        Quaternion m_KinematicTargetRotation = Quaternion::Identity;

        bool m_IsPosLocked = false;
        bool m_IsRotLocked = false;
        bool m_IsPosAxisXLocked = false;
        bool m_IsPosAxisYLocked = false;
        bool m_IsPosAxisZLocked = false;
        bool m_IsRotAxisXLocked = false;
        bool m_IsRotAxisYLocked = false;
        bool m_IsRotAxisZLocked = false;
        
        Vector<Ref<Collider>> m_ColliderCollection { CreateRef<Box>() };
        
        Vector<Entity> m_TriggerEnterBuffer;
        Vector<Entity> m_TriggerStayBuffer;
        Vector<Entity> m_TriggerExitBuffer;
        Vector<Collision> m_CollisionEnterBuffer;
        Vector<Collision> m_CollisionStayBuffer;
        Vector<Collision> m_CollisionExitBuffer;
    };
}