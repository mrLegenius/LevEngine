#pragma once
#include "physx/include/PxPhysicsAPI.h"
#include "Collider.h"
#include "DataTypes/Vector.h"
#include "Scene/Components/TypeParseTraits.h"
#include "ConstantForce.h"
#include "Scene/Components/Transform/Transform.h"

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

        static void OnConstruct(entt::registry& registry, entt::entity entity);
        static void OnDestroy(entt::registry& registry, entt::entity entity);

        bool IsInitialized() const;
        void Initialize(const Transform& transform);
        void SetRigidbodyPose(const Transform& transform);

        [[nodiscard]] bool IsVisualizationEnabled() const;
        void EnableVisualization(bool flag);

        [[nodiscard]] Type GetRigidbodyType() const;
        void SetRigidbodyType(const Type& rigidbodyType);

        //[[nodiscard]] bool IsKinematicEnabled() const;
        //void EnableKinematic(bool flag);
        
        [[nodiscard]] bool IsGravityEnabled() const;
        void EnableGravity(bool flag);
        
        [[nodiscard]] float GetMass() const;
        void SetMass(float value);
        [[nodiscard]] Vector3 GetCenterOfMass() const;
        void SetCenterOfMass(Vector3 value);
        [[nodiscard]] Vector3 GetInertiaTensor() const;
        void SetInertiaTensor(Vector3 value);

        [[nodiscard]] float GetMaxLinearVelocity() const;
        void SetMaxLinearVelocity(float value);
        [[nodiscard]] float GetMaxAngularVelocity() const;
        void SetMaxAngularVelocity(float value);
        
        [[nodiscard]] float GetLinearDamping() const;
        void SetLinearDamping(float value);
        [[nodiscard]] float GetAngularDamping() const;
        void SetAngularDamping(float value);
        
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
        
        [[nodiscard]] Collider::Type GetColliderType() const;
        void SetColliderType(const Collider::Type& colliderType);
        [[nodiscard]] int GetColliderCount() const;
        
        [[nodiscard]] Vector3 GetColliderOffsetPosition() const;
        void SetColliderOffsetPosition(Vector3 position);
        [[nodiscard]] Vector3 GetColliderOffsetRotation() const;
        void SetColliderOffsetRotation(Vector3 rotation);

        [[nodiscard]] bool IsTriggerEnabled() const;
        void EnableTrigger(bool flag);
        
        [[nodiscard]] float GetSphereRadius() const;
        void SetSphereRadius(float radius);
        [[nodiscard]] float GetCapsuleRadius() const;
        void SetCapsuleRadius(float radius);
        [[nodiscard]] float GetCapsuleHalfHeight() const;
        void SetCapsuleHalfHeight(float halfHeight);
        [[nodiscard]] Vector3 GetBoxHalfExtents() const;
        void SetBoxHalfExtents(Vector3 halfExtents);
        
        [[nodiscard]] float GetStaticFriction() const;
        void SetStaticFriction(float staticFriction);
        [[nodiscard]] float GetDynamicFriction() const;
        void SetDynamicFriction(float dynamicFriction);
        [[nodiscard]] float GetRestitution() const;
        void SetRestitution(float restitution);
        
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

        void AddForce(Vector3 force, ForceMode mode = ForceMode::Force) const;
        void AddTorque(Vector3 torque, ForceMode = ForceMode::Force) const;
        
        void OnCollisionEnter(const Action<Entity>& callback);
        void OnCollisionExit(const Action<Entity>& callback);
        void OnTriggerEnter(const Action<Entity>& callback);
        void OnTriggerExit(const Action<Entity>& callback);
        
        friend class PhysicsUpdate;
        friend class RigidbodyInitSystem;
        friend class ContactReportCallback;
        
    private:
        [[nodiscard]] physx::PxRigidActor* GetActor() const;
        [[nodiscard]] physx::PxShape* GetColliders() const;
        [[nodiscard]] physx::PxMaterial* GetPhysicalMaterials(const physx::PxShape* colliders) const;

        [[nodiscard]] Vector3 GetTransformScale() const;
        void SetTransformScale(Vector3 transformScale);

        void AttachRigidbody(const Type& rigidbodyType);
        void DetachRigidbody();
        
        void AttachCollider(const Collider::Type& colliderType);
        void DetachCollider();
        
        physx::PxRigidActor* m_Actor = nullptr;

        Vector3 m_TransformScale = Vector3::One;
        
        bool m_IsInitialized = false;
        bool m_IsVisualizationEnabled = false;

        Type m_Type = Type::Dynamic;
        
        //bool m_IsKinematicEnabled = false;
        bool m_IsGravityEnabled = true;
        
        float m_Mass = 1.0f;
        Vector3 m_CenterOfMass = Vector3::Zero;
        Vector3 m_InertiaTensor = Vector3::One;
        float m_LinearDamping = 0.0f;
        float m_AngularDamping = 0.05f;

        float m_MaxLinearVelocity = 100.0f;
        float m_MaxAngularVelocity = 100.0f;
        
        bool m_IsPosAxisXLocked = false;
        bool m_IsPosAxisYLocked = false;
        bool m_IsPosAxisZLocked = false;
        bool m_IsRotAxisXLocked = false;
        bool m_IsRotAxisYLocked = false;
        bool m_IsRotAxisZLocked = false;

        //TODO: CHANGE LOGIC FOR MULTIPLE COLLIDER ATTACHMENT
        Vector<Ref<Collider>> m_ColliderCollection { CreateRef<Box>() };

        bool m_IsTriggerEnabled = false;
        
        bool m_IsTriggerEnterEnabled = false;
        Vector<Entity> m_TriggerEnterEntityBuffer;
        bool m_IsTriggerExitEnabled = false;
        Vector<Entity> m_TriggerExitEntityBuffer;

        bool m_IsCollisionEnterEnabled = false;
        Vector<Entity> m_CollisionEnterEntityBuffer;
        bool m_IsCollisionExitEnabled = false;
        Vector<Entity> m_CollisionExitEntityBuffer;
        
        Vector<Pair<Action<Entity>,Entity>> m_ActionBuffer;
    };
}