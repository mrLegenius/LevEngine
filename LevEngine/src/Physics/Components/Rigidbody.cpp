#include "levpch.h"
#include "Rigidbody.h"

#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/Support/PhysicsUtils.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    constexpr auto MAX_COLLIDER_NUMBER = 1;
    constexpr auto MAX_MATERIAL_NUMBER = 1;

    constexpr auto DEFAULT_GRAVITY_FLAG = true;
    constexpr auto DEFAULT_COLLIDER_TYPE = Collider::Type::Box;

    
    
    void Rigidbody::OnDestroy(entt::registry& registry, entt::entity entity)
    {
        auto& rigidbody = registry.get<Rigidbody>(entity);
        App::Get().GetPhysics().m_ActorEntityMap.erase(rigidbody.GetActor());
        
        rigidbody.DetachRigidbody();
        
        //Log::Debug("RIGIDBODY DESTROYED");
        //Log::Debug("ACTOR MAP SIZE: {0}", App::Get().GetPhysics().m_ActorEntityMap.size());
    }
    
    
    
    physx::PxRigidActor* Rigidbody::GetActor() const
    {
        return m_Actor;
    }
    
    physx::PxShape* Rigidbody::GetColliders() const
    {
        physx::PxShape* colliders[MAX_COLLIDER_NUMBER];
        const auto nbColliders = m_Actor->getNbShapes();
        m_Actor->getShapes(colliders, nbColliders);

        return *colliders;
    }

    physx::PxMaterial* Rigidbody::GetPhysicalMaterials(const physx::PxShape* colliders) const
    {
        physx::PxMaterial* physicalMaterials[MAX_MATERIAL_NUMBER];
        const auto nbMaterials = colliders[0].getNbMaterials();
        colliders[0].getMaterials(physicalMaterials, nbMaterials);

        return *physicalMaterials;
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
    
    void Rigidbody::Initialize(const Transform& transform)
    {
        if (m_IsInitialized) return;
        
        AttachRigidbody(m_Type);
        SetRigidbodyPose(transform);
        
        AttachCollider(GetColliderType());
        SetColliderOffsetPosition(GetColliderOffsetPosition());
        SetColliderOffsetRotation(GetColliderOffsetRotation());
        
        m_IsInitialized = true;
        
        SetTransformScale(transform.GetWorldScale());
    }

    void Rigidbody::SetRigidbodyPose(const Transform& transform)
    {
        const physx::PxTransform pxTransform = PhysicsUtils::FromTransformToPxTransform(transform);
        m_Actor->setGlobalPose(pxTransform);
    }


    
    Rigidbody::Type Rigidbody::GetRigidbodyType() const
    {
        return m_Type;
    }

    void Rigidbody::SetRigidbodyType(const Type& rigidbodyType)
    {
        m_Type = rigidbodyType;
        
        if (m_IsInitialized)
        {
            AttachRigidbody(rigidbodyType);
            SetColliderType(DEFAULT_COLLIDER_TYPE);
            SetTransformScale(m_TransformScale);
        }
    }

    void Rigidbody::AttachRigidbody(const Type& rigidbodyType)
    {
        auto initialPose = physx::PxTransform(physx::PxIdentity);
        if (m_IsInitialized)
        {
            initialPose = m_Actor->getGlobalPose();
            DetachRigidbody();
        }
    
        switch (rigidbodyType)
        {
        case Type::Static:
            m_Actor = App::Get().GetPhysics().GetPhysics()->createRigidStatic(initialPose);
            App::Get().GetPhysics().GetScene()->addActor(*(reinterpret_cast<physx::PxRigidStatic*>(m_Actor)));
            break;
        case Type::Dynamic:
            m_Actor = App::Get().GetPhysics().GetPhysics()->createRigidDynamic(initialPose);
            App::Get().GetPhysics().GetScene()->addActor(*(reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)));
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

        if (m_IsInitialized)
        {
            App::Get().GetPhysics().m_ActorEntityMap.insert(m_Actor);
        }
        
        EnableVisualization(m_IsVisualizationEnabled);
    }

    void Rigidbody::DetachRigidbody()
    {
        DetachCollider();

        if (m_Actor != NULL)
        {
            App::Get().GetPhysics().m_ActorEntityMap.erase(m_Actor);
            
            m_Actor->getScene()->removeActor(*m_Actor);
            PX_RELEASE(m_Actor);
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
    
    
    
    Collider::Type Rigidbody::GetColliderType() const
    {
        return m_ColliderCollection[0].get()->m_Type;
    }

    void Rigidbody::SetColliderType(const Collider::Type& colliderType)
    {
        if (GetColliderCount() >= MAX_COLLIDER_NUMBER)
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

        if (m_IsInitialized)
        {
            AttachCollider(colliderType);
            SetTransformScale(m_TransformScale);
        }
    }

    void Rigidbody::AttachCollider(const Collider::Type& colliderType)
    {
        const auto physicalMaterial = App::Get().GetPhysics().GetPhysics()->createMaterial(GetStaticFriction(), GetDynamicFriction(), GetRestitution());
        
        physx::PxShape* collider = nullptr;
        switch (colliderType)
        {
        case Collider::Type::Sphere:
            collider = App::Get().GetPhysics().GetPhysics()->createShape(physx::PxSphereGeometry(GetSphereRadius()), *physicalMaterial, true);
            break;
        case Collider::Type::Capsule:
            collider = App::Get().GetPhysics().GetPhysics()->createShape(physx::PxCapsuleGeometry(GetCapsuleRadius(), GetCapsuleHalfHeight()), *physicalMaterial, true);
            break;
        case Collider::Type::Box:
            collider = App::Get().GetPhysics().GetPhysics()->createShape(physx::PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(GetBoxHalfExtents())), *physicalMaterial, true);
            break;
        default:
            break;
        }
    
        m_Actor->attachShape(*collider);
        collider->release();
        physicalMaterial->release();

        EnableTrigger(IsTriggerEnabled());
    }
    
    void Rigidbody::DetachCollider()
    {
        if (GetColliderCount() <= 0) return;

        if (m_Actor != NULL)
        {
            const auto colliders = GetColliders();
            for (auto i = 0; i < GetColliderCount(); i++)
            {
                m_Actor->detachShape(colliders[i]);
            }
        }
        
        m_ColliderCollection.clear();
    }

    int Rigidbody::GetColliderCount() const
    {
        return static_cast<int>(m_ColliderCollection.size());
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
    
    
    
    Vector3 Rigidbody::GetColliderOffsetPosition() const
    {
        return m_ColliderCollection[0]->OffsetPosition;
    }

    void Rigidbody::SetColliderOffsetPosition(const Vector3 position)
    {
        m_ColliderCollection[0]->OffsetPosition = position;
        
        if (m_Actor != NULL)
        {
            const auto colliders = GetColliders();
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
            const auto colliders = GetColliders();
            const auto position = colliders[0].getLocalPose().p;
            const auto quaternion = Quaternion::CreateFromYawPitchRoll(Math::ToRadians(rotation));
            colliders[0].setLocalPose(physx::PxTransform(position, PhysicsUtils::FromQuaternionToPxQuat(quaternion)));
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
            GetColliders()[0].setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
            GetColliders()[0].setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
            GetColliders()[0].setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !flag);
            GetColliders()[0].setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, flag);
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
            
            const auto colliders = GetColliders();
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
            
            const auto colliders = GetColliders();
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
            
            const auto colliders = GetColliders();
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
            const auto colliders = GetColliders();
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
            const auto colliders = GetColliders();
            const auto physicalMaterials = GetPhysicalMaterials(colliders);
            
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
            const auto colliders = GetColliders();
            const auto physicalMaterials = GetPhysicalMaterials(colliders);
            
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
            const auto colliders = GetColliders();
            const auto physicalMaterials = GetPhysicalMaterials(colliders);
            
            physicalMaterials[0].setRestitution(restitution);
        }
    }


    
    void Rigidbody::OnCollisionEnter(const Action<Collision>& callback)
    {
        m_IsCollisionEnterEnabled = true;
        
        while (!m_CollisionEnterEntityBuffer.empty())
        {
            //Log::Debug("START m_CollisionEnterEntityBuffer SIZE: {0}", m_CollisionEnterEntityBuffer.size());
            // TODO: IMPLEMENT NORMAL ENTITY VALID CHECK
            if (m_CollisionEnterEntityBuffer.back().ContactEntity)
            {
                const auto& collisionInfo = m_CollisionEnterEntityBuffer.back();
                App::Get().GetPhysics().m_CollisionEvents.push_back(MakePair<Action<Collision>,Collision>(callback, collisionInfo));
                //Log::Debug("Collision Enter event was added");
            }
            m_CollisionEnterEntityBuffer.pop_back();
            //Log::Debug("END m_CollisionEnterEntityBuffer SIZE: {0}", m_CollisionEnterEntityBuffer.size());
        }
    }
    
    void Rigidbody::OnCollisionExit(const Action<Collision>& callback)
    {
        m_IsCollisionExitEnabled = true;
        
        while (!m_CollisionExitEntityBuffer.empty())
        {
            //Log::Debug("START m_CollisionExitEntityBuffer SIZE: {0}", m_CollisionExitEntityBuffer.size());
            // TODO: IMPLEMENT NORMAL ENTITY VALID CHECK
            if (m_CollisionExitEntityBuffer.back().ContactEntity)
            {
                const auto& collisionInfo = m_CollisionExitEntityBuffer.back();
                App::Get().GetPhysics().m_CollisionEvents.push_back(MakePair<Action<Collision>,Collision>(callback, collisionInfo));
                //Log::Debug("Collision Exit event was added");
            }
            m_CollisionExitEntityBuffer.pop_back();
            //Log::Debug("END m_CollisionExitEntityBuffer SIZE: {0}", m_CollisionExitEntityBuffer.size());
        }
    }

    void Rigidbody::OnTriggerEnter(const Action<Entity>& callback)
    {
        m_IsTriggerEnterEnabled = true;
        
        while (!m_TriggerEnterEntityBuffer.empty())
        {
            //Log::Debug("START m_TriggerEnterEntityBuffer SIZE: {0}", m_TriggerEnterEntityBuffer.size());
            // TODO: IMPLEMENT NORMAL ENTITY VALID CHECK
            if (m_TriggerEnterEntityBuffer.back())
            {
                const auto& otherEntity = m_TriggerEnterEntityBuffer.back();
                App::Get().GetPhysics().m_TriggerEvents.push_back(MakePair<Action<Entity>,Entity>(callback, otherEntity));
                //Log::Debug("Trigger Enter event was added");
            }
            m_TriggerEnterEntityBuffer.pop_back();
            //Log::Debug("END m_TriggerEnterEntityBuffer SIZE: {0}", m_TriggerEnterEntityBuffer.size());
        }
    }

    void Rigidbody::OnTriggerExit(const Action<Entity>& callback)
    {
        m_IsTriggerExitEnabled = true;
        
        while (!m_TriggerExitEntityBuffer.empty())
        {
            //Log::Debug("START m_TriggerExitEntityBuffer SIZE: {0}", m_TriggerExitEntityBuffer.size());
            // TODO: IMPLEMENT NORMAL ENTITY VALID CHECK
            if (m_TriggerExitEntityBuffer.back())
            {
                const auto& otherEntity = m_TriggerExitEntityBuffer.back();
                App::Get().GetPhysics().m_TriggerEvents.push_back(MakePair<Action<Entity>,Entity>(callback, otherEntity));
                //Log::Debug("Trigger Exit event was added");
            }
            m_TriggerExitEntityBuffer.pop_back();
            //Log::Debug("END m_TriggerExitEntityBuffer SIZE: {0}", m_TriggerExitEntityBuffer.size());
        }
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
            
            if (const auto rigidbodyTypeNode = node["Rigidbody Type"])
            {
                component.SetRigidbodyType(static_cast<Rigidbody::Type>(rigidbodyTypeNode.as<int>()));
            }

            /*
            if (const auto kinematicEnableNode = node["Is Kinematic Enabled"])
            {
                component.EnableKinematic(kinematicEnableNode.as<bool>());
            }
            */

            if (const auto gravityEnableNode = node["Is Gravity Enabled"])
            {
                component.EnableGravity(gravityEnableNode.as<bool>());
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

            if (const auto linearDampingNode = node["Linear Damping"])
            {
                component.SetLinearDamping(linearDampingNode.as<float>());
            }

            if (const auto angularDampingNode = node["Angular Damping"])
            {
                component.SetAngularDamping(angularDampingNode.as<float>());
            }

            if (const auto maxLinearVelocityNode = node["Max Linear Velocity"])
            {
                component.SetMaxLinearVelocity(maxLinearVelocityNode.as<float>());
            }

            if (const auto maxAngularVelocityNode = node["Max Angular Velocity"])
            {
                component.SetMaxAngularVelocity(maxAngularVelocityNode.as<float>());
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

            if (const auto triggerEnableNode = node["Is Trigger Enabled"])
            {
                component.EnableTrigger(triggerEnableNode.as<bool>());
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