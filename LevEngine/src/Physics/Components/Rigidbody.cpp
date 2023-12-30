#include "levpch.h"
#include "Rigidbody.h"

#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsUtils.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    void Rigidbody::OnDestroy(entt::registry& registry, entt::entity entity)
    {
        auto& rigidbody = registry.get<Rigidbody>(entity);
        if (rigidbody.GetActor() != NULL)
        {
            rigidbody.DetachRigidbody();
        }
    }
    
    physx::PxRigidActor* Rigidbody::GetActor() const
    {
        return m_Actor;
    }
    
    physx::PxShape* Rigidbody::GetCollider() const
    {
        physx::PxShape* collider[1];
        const auto nbColliders = m_Actor->getNbShapes();
        m_Actor->getShapes(collider, nbColliders);

        return *collider;
    }

    physx::PxMaterial* Rigidbody::GetPhysicalMaterial() const
    {
        physx::PxMaterial* physicalMaterial[1];
        const auto nbMaterials = GetCollider()->getNbMaterials();
        GetCollider()->getMaterials(physicalMaterial, nbMaterials);

        return *physicalMaterial;
    }
    
    Vector3 Rigidbody::GetTransformScale() const
    {
        return m_TransformScale;
    }

    void Rigidbody::SetTransformScale(const Vector3 transformScale)
    {
        if (m_TransformScale != transformScale)
        {
            m_TransformScale = transformScale;
        }

        if (m_IsInitialized)
        {
            switch(GetColliderType())
            {
            case Collider::Type::Sphere:
                SetSphereRadius(GetSphereRadius());
                break;
            case Collider::Type::Capsule:
                SetCapsuleRadius(GetCapsuleRadius());
                SetCapsuleHalfHeight(GetCapsuleHalfHeight());
                break;
            case Collider::Type::Box:
                SetBoxHalfExtents(GetBoxHalfExtents());
                break;
            default:
                break;
            }
        }
    }
    
    bool Rigidbody::IsInitialized() const
    {
        return m_IsInitialized;
    }
    
    void Rigidbody::Initialize(const Entity entity)
    {
        if (m_IsInitialized) return;
        
        AttachRigidbody(entity);
        AttachCollider();
        
        m_IsInitialized = true;

        // PhysX Transform doesn't take into account the object scale
        const auto& transform = entity.GetComponent<Transform>();
        SetTransformScale(transform.GetWorldScale());
    }
    
    bool Rigidbody::IsVisualizationEnabled() const
    {
        return m_IsVisualizationEnabled;
    }
    
    void Rigidbody::EnableVisualization(const bool flag)
    {
        m_IsVisualizationEnabled = flag;
        
        if (m_Actor != NULL)
        {
            m_Actor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, flag);
        }
    }

    bool Rigidbody::IsGravityEnabled() const
    {
        return m_IsGravityEnabled;
    }
    
    void Rigidbody::EnableGravity(const bool flag)
    {
        m_IsGravityEnabled = flag;
        
        if (m_Actor != NULL)
        {
            m_Actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !flag);
        }
    }

    bool Rigidbody::IsKinematicEnabled() const
    {
        return m_IsKinematicEnabled;
    }

    void Rigidbody::EnableKinematic(const bool flag)
    {
        m_IsKinematicEnabled = flag;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidBody*>(m_Actor)->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, flag);
        }
    }
    
    Rigidbody::Type Rigidbody::GetRigidbodyType() const
    {
        return m_Type;
    }

    void Rigidbody::SetRigidbodyType(const Type& rigidbodyType)
    {
        m_Type = rigidbodyType;
        
        if (m_Actor != NULL)
        {
            // copy old Entity
            const auto entity = App::Get().GetPhysics().m_ActorEntityMap.at(m_Actor);
            
            AttachRigidbody(entity);
            SetColliderType(GetColliderType());
        }
    }
    
    void Rigidbody::AttachRigidbody(const Entity entity)
    {
        if (m_Actor != NULL)
        {
            DetachRigidbody();
        }
    
        switch (m_Type)
        {
        case Type::Static:
            m_Actor = App::Get().GetPhysics().CreateStaticActor(entity);
            break;
        case Type::Dynamic:
            m_Actor = App::Get().GetPhysics().CreateDynamicActor(entity);
            EnableGravity(m_IsGravityEnabled);
            EnableKinematic(m_IsKinematicEnabled);
            SetMass(m_Mass);
            SetCenterOfMass(m_CenterOfMass);
            SetInertiaTensor(m_InertiaTensor);
            SetLinearDamping(m_LinearDamping);
            SetAngularDamping(m_AngularDamping);
            SetMaxLinearVelocity(m_MaxLinearVelocity);
            SetMaxAngularVelocity(m_MaxAngularVelocity);
            LockPosAxisX(m_IsPosAxisXLocked);
            LockPosAxisY(m_IsPosAxisYLocked);
            LockPosAxisZ(m_IsPosAxisZLocked);
            LockRotAxisX(m_IsRotAxisXLocked);
            LockRotAxisY(m_IsRotAxisYLocked);
            LockRotAxisZ(m_IsRotAxisZLocked);
            break;
        default:
            break;
        }
        
        EnableVisualization(m_IsVisualizationEnabled);
    }

    void Rigidbody::DetachRigidbody()
    {
        if (GetColliderCount() > 0)
        {
            DetachCollider();
        }
        App::Get().GetPhysics().RemoveActor(m_Actor);
    }

    float Rigidbody::GetMass() const
    {
        return m_Mass;
    }

    void Rigidbody::SetMass(const float value)
    {
        if (value < 0.0f) return;
        
        if (m_Type != Type::Dynamic) return;
        
        m_Mass = value;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)->setMass(value);
        }
    }

    Vector3 Rigidbody::GetCenterOfMass() const
    {
        return m_CenterOfMass;
    }

    void Rigidbody::SetCenterOfMass(const Vector3 value)
    {
        if (m_Type != Type::Dynamic) return;

        m_CenterOfMass = value;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)->setCMassLocalPose(physx::PxTransform(PhysicsUtils::FromVector3ToPxVec3(value)));
        }
    }

    Vector3 Rigidbody::GetInertiaTensor() const
    {
        return m_InertiaTensor;
    }

    void Rigidbody::SetInertiaTensor(const Vector3 value)
    {
        if (value.x < 0.0f || value.y < 0.0f || value.z < 0.0f) return;
        
        if (m_Type != Type::Dynamic) return;

        m_InertiaTensor = value;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)->setMassSpaceInertiaTensor(PhysicsUtils::FromVector3ToPxVec3(value));
        }
    }

    float Rigidbody::GetMaxLinearVelocity() const
    {
        return m_MaxLinearVelocity;
    }

    void Rigidbody::SetMaxLinearVelocity(const float value)
    {
        if (value < 0.0f) return;
        
        if (m_Type != Type::Dynamic) return;
        
        m_MaxLinearVelocity = value;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidBody*>(m_Actor)->setMaxLinearVelocity(value);
        }
    }

    float Rigidbody::GetMaxAngularVelocity() const
    {
        return m_MaxAngularVelocity;
    }

    void Rigidbody::SetMaxAngularVelocity(const float value)
    {
        if (value < 0.0f) return;
        
        if (m_Type != Type::Dynamic) return;
        
        m_MaxAngularVelocity = value;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidBody*>(m_Actor)->setMaxAngularVelocity(value);
        }
    }
    
    float Rigidbody::GetLinearDamping() const
    {
        return m_LinearDamping;
    }

    void Rigidbody::SetLinearDamping(const float value)
    {
        if (value < 0.0f) return;
        
        if (m_Type != Type::Dynamic) return;
        
        m_LinearDamping = value;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)->setLinearDamping(value);
        }
    }

    float Rigidbody::GetAngularDamping() const
    {
        return m_AngularDamping;
    }

    void Rigidbody::SetAngularDamping(const float value)
    {
        if (value < 0.0f) return;
        
        if (m_Type != Type::Dynamic) return;
        
        m_AngularDamping = value;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)->setAngularDamping(value);
        }
    }
    
    bool Rigidbody::IsPosAxisXLocked() const
    {
        return m_IsPosAxisXLocked;
    }

    void Rigidbody::LockPosAxisX(const bool flag)
    {
        if (m_Type != Type::Dynamic) return;
        
        m_IsPosAxisXLocked = flag;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, flag);
        }
    }

    bool Rigidbody::IsPosAxisYLocked() const
    {
        return m_IsPosAxisYLocked;
    }

    void Rigidbody::LockPosAxisY(const bool flag)
    {
        if (m_Type != Type::Dynamic) return;
        
        m_IsPosAxisYLocked = flag;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, flag);
        }
    }

    bool Rigidbody::IsPosAxisZLocked() const
    {
        return m_IsPosAxisZLocked;
    }

    void Rigidbody::LockPosAxisZ(const bool flag)
    {
        if (m_Type != Type::Dynamic) return;
        
        m_IsPosAxisZLocked = flag;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, flag);
        }
    }

    bool Rigidbody::IsRotAxisXLocked() const
    {
        return m_IsRotAxisXLocked;
    }

    void Rigidbody::LockRotAxisX(const bool flag)
    {
        if (m_Type != Type::Dynamic) return;
        
        m_IsRotAxisXLocked = flag;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, flag);
        }
    }

    bool Rigidbody::IsRotAxisYLocked() const
    {
        return m_IsRotAxisYLocked;
    }

    void Rigidbody::LockRotAxisY(const bool flag)
    {
        if (m_Type != Type::Dynamic) return;
        
        m_IsRotAxisYLocked = flag;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, flag);
        }
    }

    bool Rigidbody::IsRotAxisZLocked() const
    {
        return m_IsRotAxisZLocked;
    }

    void Rigidbody::LockRotAxisZ(const bool flag)
    {
        if (m_Type != Type::Dynamic) return;
        
        m_IsRotAxisZLocked = flag;

        if (m_Actor != NULL)
        {
            reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, flag);
        }
    }

    bool Rigidbody::IsTriggerEnabled() const
    {
        return m_ColliderCollection[0]->m_IsTriggerEnabled;
    }

    void Rigidbody::EnableTrigger(bool flag)
    {
        m_ColliderCollection[0]->m_IsTriggerEnabled = flag;

        if (m_Actor != NULL)
        {
            GetCollider()[0].setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
            GetCollider()[0].setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
            GetCollider()[0].setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !flag);
            GetCollider()[0].setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, flag);
        }
    }
    
    Collider::Type Rigidbody::GetColliderType() const
    {
        return m_ColliderCollection[0].get()->m_Type;
    }

    void Rigidbody::SetColliderType(const Collider::Type& colliderType)
    {
        if (m_Actor != NULL && GetColliderCount() > 0)
        {
            DetachCollider();
        }
        
        switch (colliderType)
        {
        case Collider::Type::Sphere:
            m_ColliderCollection.push_back(CreateRef<Sphere>());
            break;
        case Collider::Type::Capsule:
            m_ColliderCollection.push_back(CreateRef<Capsule>());
            break;
        case Collider::Type::Box:
            m_ColliderCollection.push_back(CreateRef<Box>());
            break;
        default:
            break;
        }
        m_ColliderCollection[0].get()->m_Type = colliderType;

        if (m_Actor != NULL)
        {
            AttachCollider();
            SetTransformScale(m_TransformScale);
        }
    }

    void Rigidbody::AttachCollider()
    {
        const auto physicalMaterial =
            App::Get().GetPhysics().CreateMaterial(
                GetStaticFriction(),
                GetDynamicFriction(),
                GetRestitution()
            );
        
        physx::PxShape* collider = nullptr;
        switch (GetColliderType())
        {
        case Collider::Type::Sphere:
            collider =
                App::Get().GetPhysics().CreateSphere(
                    GetSphereRadius(),
                    physicalMaterial
                );
            break;
        case Collider::Type::Capsule:
            collider =
                App::Get().GetPhysics().CreateCapsule(
                    GetCapsuleRadius(),
                    GetCapsuleHalfHeight(),
                    physicalMaterial
                );
            break;
        case Collider::Type::Box:
            collider =
                App::Get().GetPhysics().CreateBox(
                    GetBoxHalfExtents(),
                    physicalMaterial
                );
            break;
        default:
            break;
        }
    
        m_Actor->attachShape(*collider);
        
        collider->release();
        physicalMaterial->release();

        EnableTrigger(IsTriggerEnabled());
        SetColliderOffsetPosition(GetColliderOffsetPosition());
        SetColliderOffsetRotation(GetColliderOffsetRotation());
    }
    
    void Rigidbody::DetachCollider()
    {
        const auto colliders = GetCollider();
        m_Actor->detachShape(colliders[0]);
        
        m_ColliderCollection.clear();
    }

    int Rigidbody::GetColliderCount() const
    {
        return static_cast<int>(m_ColliderCollection.size());
    }
    
    Vector3 Rigidbody::GetColliderOffsetPosition() const
    {
        return m_ColliderCollection[0]->OffsetPosition;
    }

    void Rigidbody::SetColliderOffsetPosition(const Vector3 position)
    {
        m_ColliderCollection[0]->OffsetPosition = position;
        
        if (m_Actor != NULL)
        {
            const auto colliders = GetCollider();
            const auto rotation = colliders[0].getLocalPose().q;
            colliders[0].setLocalPose(physx::PxTransform(PhysicsUtils::FromVector3ToPxVec3(position), rotation));
        }
    }

    Vector3 Rigidbody::GetColliderOffsetRotation() const
    {
        return m_ColliderCollection[0]->OffsetRotation;
    }

    void Rigidbody::SetColliderOffsetRotation(const Vector3 rotation)
    {
        m_ColliderCollection[0]->OffsetRotation = rotation;
        
        if (m_Actor != NULL)
        {
            const auto colliders = GetCollider();
            const auto position = colliders[0].getLocalPose().p;
            const auto quaternion = Quaternion::CreateFromYawPitchRoll(Math::ToRadians(rotation));
            colliders[0].setLocalPose(physx::PxTransform(position, PhysicsUtils::FromQuaternionToPxQuat(quaternion)));
        }
    }
    
    float Rigidbody::GetSphereRadius() const
    {
        return static_cast<Sphere*>(m_ColliderCollection[0].get())->Radius;
    }

    void Rigidbody::SetSphereRadius(const float radius)
    {
        if (radius <= 0.0f) return;
        
        static_cast<Sphere*>(m_ColliderCollection[0].get())->Radius = radius;
        
        if (m_Actor != NULL)
        {
            const float maxTransformScale = Math::MaxElement(m_TransformScale);
            
            const auto colliders = GetCollider();
            colliders[0].setGeometry(physx::PxSphereGeometry(radius * maxTransformScale));
        }
    }

    float Rigidbody::GetCapsuleRadius() const
    {
        return static_cast<Capsule*>(m_ColliderCollection[0].get())->Radius;
    }

    void Rigidbody::SetCapsuleRadius(const float radius)
    {
        if (radius <= 0.0f) return;
        
        static_cast<Capsule*>(m_ColliderCollection[0].get())->Radius = radius;
        
        if (m_Actor != NULL)
        {
            const float maxTransformScale = Math::MaxElement(m_TransformScale);
            
            const auto colliders = GetCollider();
            const physx::PxGeometryHolder geom(colliders[0].getGeometry());
            const auto initialHeight = geom.capsule().halfHeight;
            colliders[0].setGeometry(physx::PxCapsuleGeometry(radius * maxTransformScale, initialHeight));
        }
    }
    
    float Rigidbody::GetCapsuleHalfHeight() const
    {
        return static_cast<Capsule*>(m_ColliderCollection[0].get())->HalfHeight;
    }

    void Rigidbody::SetCapsuleHalfHeight(const float halfHeight)
    {
        if (halfHeight < 0.0f) return;
        
        static_cast<Capsule*>(m_ColliderCollection[0].get())->HalfHeight = halfHeight;
        
        if (m_Actor != NULL)
        {
            const float maxTransformScale = Math::MaxElement(m_TransformScale);
            
            const auto colliders = GetCollider();
            const physx::PxGeometryHolder geom(colliders[0].getGeometry());
            const auto initialRadius = geom.capsule().radius;
            colliders[0].setGeometry(physx::PxCapsuleGeometry(initialRadius, halfHeight * maxTransformScale));
        }
    }

    Vector3 Rigidbody::GetBoxHalfExtents() const
    {
        return static_cast<Box*>(m_ColliderCollection[0].get())->HalfExtents;
    }

    void Rigidbody::SetBoxHalfExtents(const Vector3 halfExtents)
    {
        if (halfExtents.x <= 0.0f || halfExtents.y <= 0.0f || halfExtents.z <= 0.0f) return;
        
        static_cast<Box*>(m_ColliderCollection[0].get())->HalfExtents = halfExtents;
        
        if (m_Actor != NULL)
        {
            const auto colliders = GetCollider();
            colliders[0].setGeometry(physx::PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(halfExtents * m_TransformScale)));
        }
    }
    
    float Rigidbody::GetStaticFriction() const
    {
        return m_ColliderCollection[0]->m_PhysicalMaterial->StaticFriction;
    }

    void Rigidbody::SetStaticFriction(const float staticFriction)
    {
        if (staticFriction < 0.0f || staticFriction > 1.0f) return;
        
        m_ColliderCollection[0]->m_PhysicalMaterial->StaticFriction = staticFriction;
        
        if (m_Actor != NULL)
        {
            const auto physicalMaterials = GetPhysicalMaterial();
            physicalMaterials[0].setStaticFriction(staticFriction);
        }
    }
    
    float Rigidbody::GetDynamicFriction() const
    {
        return m_ColliderCollection[0]->m_PhysicalMaterial->DynamicFriction;
    }

    void Rigidbody::SetDynamicFriction(const float dynamicFriction)
    {
        if (dynamicFriction < 0.0f || dynamicFriction > 1.0f) return;
        
        m_ColliderCollection[0]->m_PhysicalMaterial->DynamicFriction = dynamicFriction;
        
        if (m_Actor != NULL)
        {
            const auto physicalMaterials = GetPhysicalMaterial();
            physicalMaterials[0].setDynamicFriction(dynamicFriction);
        }
    }
    
    float Rigidbody::GetRestitution() const
    {
        return m_ColliderCollection[0]->m_PhysicalMaterial->Restitution;
    }

    void Rigidbody::SetRestitution(const float restitution)
    {
        if (restitution < 0.0f || restitution > 1.0f) return;
        
        m_ColliderCollection[0]->m_PhysicalMaterial->Restitution = restitution;
        
        if (m_Actor != NULL)
        {
            const auto physicalMaterials = GetPhysicalMaterial();
            physicalMaterials[0].setRestitution(restitution);
        }
    }

    void Rigidbody::AddForce(const Vector3 force, const ForceMode mode) const
    {
        if (m_Type != Type::Dynamic) return;

        if (m_Actor != NULL)
        {
            if (const auto rigidDynamic = reinterpret_cast<physx::PxRigidDynamic*>(m_Actor))
            {
                if (mode == ForceMode::Force)
                {
                    rigidDynamic->addForce(PhysicsUtils::FromVector3ToPxVec3(force), physx::PxForceMode::eFORCE);
                }
                else if (mode == ForceMode::Impulse)
                {
                    rigidDynamic->addForce(PhysicsUtils::FromVector3ToPxVec3(force), physx::PxForceMode::eIMPULSE);
                }
                else if (mode == ForceMode::Acceleration)
                {
                    rigidDynamic->addForce(PhysicsUtils::FromVector3ToPxVec3(force), physx::PxForceMode::eACCELERATION);
                }
                else if (mode == ForceMode::VelocityChange)
                {
                    rigidDynamic->addForce(PhysicsUtils::FromVector3ToPxVec3(force), physx::PxForceMode::eVELOCITY_CHANGE);
                }
            }
        }
    }
    
    void Rigidbody::AddTorque(const Vector3 torque, const ForceMode mode) const
    {
        if (m_Type != Type::Dynamic) return;

        if (m_Actor != NULL)
        {
            if (const auto rigidDynamic = reinterpret_cast<physx::PxRigidDynamic*>(m_Actor))
            {
                if (mode == ForceMode::Force)
                {
                    rigidDynamic->addTorque(PhysicsUtils::FromVector3ToPxVec3(torque), physx::PxForceMode::eFORCE);
                }
                else if (mode == ForceMode::Impulse)
                {
                    rigidDynamic->addTorque(PhysicsUtils::FromVector3ToPxVec3(torque), physx::PxForceMode::eIMPULSE);
                }
                else if (mode == ForceMode::Acceleration)
                {
                    rigidDynamic->addTorque(PhysicsUtils::FromVector3ToPxVec3(torque), physx::PxForceMode::eACCELERATION);
                }
                else if (mode == ForceMode::VelocityChange)
                {
                    rigidDynamic->addTorque(PhysicsUtils::FromVector3ToPxVec3(torque), physx::PxForceMode::eVELOCITY_CHANGE);
                }
            }
        }
    }

    const Vector<Entity>& Rigidbody::GetTriggerEnterBuffer() const
    {
        return m_TriggerEnterBuffer;
    }

    const Vector<Entity>& Rigidbody::GetTriggerExitBuffer() const
    {
        return m_TriggerExitBuffer;
    }

    const Vector<Collision>& Rigidbody::GetCollisionEnterBuffer() const
    {
        return m_CollisionEnterBuffer; 
    }

    const Vector<Collision>& Rigidbody::GetCollisionExitBuffer() const
    {
        return m_CollisionExitBuffer;
    }
    
    class RigidbodySerializer final : public ComponentSerializer<Rigidbody, RigidbodySerializer>
    {
    protected:
        const char* GetKey() override { return "Rigidbody"; }

        void SerializeData(YAML::Emitter& out, const Rigidbody& component) override
        {
            out << YAML::Key << "Is Visualization Enabled" << YAML::Value << component.IsVisualizationEnabled();
            
            out << YAML::Key << "Rigidbody Type" << YAML::Value << static_cast<int>(component.GetRigidbodyType());
            
            //out << YAML::Key << "Is Kinematic Enabled" << YAML::Value << component.IsKinematicEnabled();
            
            out << YAML::Key << "Is Gravity Enabled" << YAML::Value << component.IsGravityEnabled();
            
            out << YAML::Key << "Mass" << YAML::Value << component.GetMass();
            out << YAML::Key << "Center Of Mass" << YAML::Value << component.GetCenterOfMass();
            out << YAML::Key << "Inertia Tensor" << YAML::Value << component.GetInertiaTensor();
            out << YAML::Key << "Linear Damping" << YAML::Value << component.GetLinearDamping();
            out << YAML::Key << "Angular Damping" << YAML::Value << component.GetAngularDamping();

            out << YAML::Key << "Max Linear Velocity" << YAML::Value << component.GetMaxLinearVelocity();
            out << YAML::Key << "Max Angular Velocity" << YAML::Value << component.GetMaxAngularVelocity();
            
            out << YAML::Key << "Is Pos Axis X Locked" << YAML::Value << component.IsPosAxisXLocked();
            out << YAML::Key << "Is Pos Axis Y Locked" << YAML::Value << component.IsPosAxisYLocked();
            out << YAML::Key << "Is Pos Axis Z Locked" << YAML::Value << component.IsPosAxisZLocked();
            out << YAML::Key << "Is Rot Axis X Locked" << YAML::Value << component.IsRotAxisXLocked();
            out << YAML::Key << "Is Rot Axis Y Locked" << YAML::Value << component.IsRotAxisYLocked();
            out << YAML::Key << "Is Rot Axis Z Locked" << YAML::Value << component.IsRotAxisZLocked();
            
            out << YAML::Key << "Collider Type" << YAML::Value << static_cast<int>(component.GetColliderType());
            switch (component.GetColliderType())
            {
            case Collider::Type::Sphere:
                out << YAML::Key << "Sphere Radius" << YAML::Value << component.GetSphereRadius();
                break;
            case Collider::Type::Capsule:
                out << YAML::Key << "Capsule Radius" << YAML::Value << component.GetCapsuleRadius();
                out << YAML::Key << "Capsule Half Height" << YAML::Value << component.GetCapsuleHalfHeight();
                break;
            case Collider::Type::Box:
                out << YAML::Key << "Box Half Extends" << YAML::Value << component.GetBoxHalfExtents();
                break;
            default:
                break;
            }

            out << YAML::Key << "Offset Position" << YAML::Value << component.GetColliderOffsetPosition();
            out << YAML::Key << "Offset Rotation" << YAML::Value << component.GetColliderOffsetRotation();

            out << YAML::Key << "Is Trigger Enabled" << YAML::Value << component.IsTriggerEnabled();
            
            out << YAML::Key << "Static Friction" << YAML::Value << component.GetStaticFriction();
            out << YAML::Key << "Dynamic Friction" << YAML::Value << component.GetDynamicFriction();
            out << YAML::Key << "Restitution" << YAML::Value << component.GetRestitution();
        }

        void DeserializeData(YAML::Node& node, Rigidbody& component) override
        {
            if (const auto visualizationEnableNode = node["Is Visualization Enabled"])
            {
                component.EnableVisualization(visualizationEnableNode.as<bool>());
            }

            if (const auto gravityEnableNode = node["Is Gravity Enabled"])
            {
                component.EnableGravity(gravityEnableNode.as<bool>());
            }

            /*
            if (const auto kinematicEnableNode = node["Is Kinematic Enabled"])
            {
                component.EnableKinematic(kinematicEnableNode.as<bool>());
            }
            */
            
            if (const auto rigidbodyTypeNode = node["Rigidbody Type"])
            {
                component.SetRigidbodyType(static_cast<Rigidbody::Type>(rigidbodyTypeNode.as<int>()));
            }
            
            if (const auto massNode = node["Mass"])
            {
                component.SetMass(massNode.as<float>());
            }
            
            if (const auto centerOfMassNode = node["Center Of Mass"])
            {
                component.SetCenterOfMass(centerOfMassNode.as<Vector3>());
            }

            if (const auto inertiaTensorNode = node["Inertia Tensor"])
            {
                component.SetInertiaTensor(inertiaTensorNode.as<Vector3>());
            }

            if (const auto maxLinearVelocityNode = node["Max Linear Velocity"])
            {
                component.SetMaxLinearVelocity(maxLinearVelocityNode.as<float>());
            }

            if (const auto maxAngularVelocityNode = node["Max Angular Velocity"])
            {
                component.SetMaxAngularVelocity(maxAngularVelocityNode.as<float>());
            }

            if (const auto linearDampingNode = node["Linear Damping"])
            {
                component.SetLinearDamping(linearDampingNode.as<float>());
            }

            if (const auto angularDampingNode = node["Angular Damping"])
            {
                component.SetAngularDamping(angularDampingNode.as<float>());
            }
            
            if (const auto posAxisXLockNode = node["Is Pos Axis X Locked"])
            {
                component.LockPosAxisX(posAxisXLockNode.as<bool>());
            }

            if (const auto posAxisYLockNode = node["Is Pos Axis Y Locked"])
            {
                component.LockPosAxisY(posAxisYLockNode.as<bool>());
            }

            if (const auto posAxisZLockNode = node["Is Pos Axis Z Locked"])
            {
                component.LockPosAxisZ(posAxisZLockNode.as<bool>());
            }

            if (const auto rotAxisXLockNode = node["Is Rot Axis X Locked"])
            {
                component.LockRotAxisX(rotAxisXLockNode.as<bool>());
            }

            if (const auto rotAxisYLockNode = node["Is Rot Axis Y Locked"])
            {
                component.LockRotAxisY(rotAxisYLockNode.as<bool>());
            }

            if (const auto rotAxisZLockNode = node["Is Rot Axis Z Locked"])
            {
                component.LockRotAxisZ(rotAxisZLockNode.as<bool>());
            }

            if (const auto triggerEnableNode = node["Is Trigger Enabled"])
            {
                component.EnableTrigger(triggerEnableNode.as<bool>());
            }
            
            if (const auto colliderTypeNode = node["Collider Type"])
            {
                switch (const auto colliderType = static_cast<Collider::Type>(colliderTypeNode.as<int>()))
                {
                case Collider::Type::Sphere:
                    component.SetColliderType(colliderType);
                    if (const auto sphereRadiusNode = node["Sphere Radius"])
                    {
                        component.SetSphereRadius(sphereRadiusNode.as<float>());
                    }
                    break;
                case Collider::Type::Capsule:
                    component.SetColliderType(colliderType);
                    if (const auto capsuleRadiusNode = node["Capsule Radius"])
                    {
                        component.SetCapsuleRadius(capsuleRadiusNode.as<float>());
                    }
                    if (const auto capsuleHalfHeightNode = node["Capsule Half Height"])
                    {
                        component.SetCapsuleHalfHeight(capsuleHalfHeightNode.as<float>());
                    }
                    break;
                case Collider::Type::Box:
                    component.SetColliderType(colliderType);
                    if (const auto boxExtents = node["Box Half Extends"])
                    {
                        component.SetBoxHalfExtents(boxExtents.as<Vector3>());
                    }
                    break;
                default:
                    break;
                }
            }

            if (const auto colliderOffsetPositionNode = node["Offset Position"])
            {
                component.SetColliderOffsetPosition(colliderOffsetPositionNode.as<Vector3>());
            }

            if (const auto collideOffsetRotationNode = node["Offset Rotation"])
            {
                component.SetColliderOffsetRotation(collideOffsetRotationNode.as<Vector3>());
            }

            if (const auto staticFrictionNode = node["Static Friction"])
            {
                component.SetStaticFriction(staticFrictionNode.as<float>());
            }

            if (const auto dynamicFrictionNode = node["Dynamic Friction"])
            {
                component.SetDynamicFriction(dynamicFrictionNode.as<float>());
            }

            if (const auto restitutionNode = node["Restitution"])
            {
                component.SetRestitution(restitutionNode.as<float>());
            }
        } 
    };
}