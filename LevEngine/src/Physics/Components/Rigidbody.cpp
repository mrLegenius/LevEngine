#include "levpch.h"
#include "Rigidbody.h"

#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsUtils.h"
#include "Scene/Components/ComponentSerializer.h"

constexpr auto MAX_COLLIDER_NUMBER = 1;
constexpr auto MAX_MATERIAL_NUMBER = 1;

constexpr auto DEFAULT_RIGIDBODY_DENSITY = 10;
constexpr auto DEFAULT_GRAVITY_FLAG = true;
constexpr auto DEFAULT_COLLIDER_TYPE = Collider::Type::Box;

namespace LevEngine
{
    void Rigidbody::OnDestroy(entt::registry& registry, entt::entity entity)
    {
        auto& rigidbody = registry.get<Rigidbody>(entity);
        rigidbody.DetachRigidbody();
    }

    

    physx::PxRigidActor* Rigidbody::GetRigidbody() const
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

        if (IsInitialized())
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
        EnableVisualization(m_IsVisualizationEnabled);
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
        
        if (IsInitialized())
        {
            AttachRigidbody(rigidbodyType);
            SetColliderType(DEFAULT_COLLIDER_TYPE);
            SetTransformScale(m_TransformScale);
        }
    }

    void Rigidbody::AttachRigidbody(const Type& rigidbodyType)
    {
        auto initialPose = physx::PxTransform(physx::PxIdentity);
        if (m_Actor != NULL)
        {
            initialPose = m_Actor->getGlobalPose();
            DetachRigidbody();
        }
    
        switch (rigidbodyType)
        {
        case Type::Static:
            m_Actor = App::Get().GetPhysics().GetPhysics()->createRigidStatic(initialPose);
            App::Get().GetPhysics().GetScene()->addActor(*(reinterpret_cast<physx::PxRigidStatic*>(m_Actor)));
            EnableGravity(!DEFAULT_GRAVITY_FLAG);
            break;
        case Type::Dynamic:
            m_Actor = App::Get().GetPhysics().GetPhysics()->createRigidDynamic(initialPose);
            physx::PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)), DEFAULT_RIGIDBODY_DENSITY);
            App::Get().GetPhysics().GetScene()->addActor(*(reinterpret_cast<physx::PxRigidDynamic*>(m_Actor)));
            EnableGravity(m_IsGravityEnabled);
            break;
        default:
            break;
        }

        EnableVisualization(IsVisualizationEnabled());
    }

    void Rigidbody::DetachRigidbody()
    {
        DetachCollider();

        if (m_Actor != NULL)
        {
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

        if (IsInitialized())
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
        return m_ColliderCollection[0]->PhysicalMaterial.StaticFriction;
    }

    void Rigidbody::SetStaticFriction(const float staticFriction)
    {
        if (staticFriction < 0.0f) return;
        
        m_ColliderCollection[0]->PhysicalMaterial.StaticFriction = staticFriction;
        
        if (m_Actor != NULL)
        {
            const auto colliders = GetColliders();
            const auto physicalMaterials = GetPhysicalMaterials(colliders);
            
            physicalMaterials[0].setStaticFriction(staticFriction);
        }
    }
    
    float Rigidbody::GetDynamicFriction() const
    {
        return m_ColliderCollection[0]->PhysicalMaterial.DynamicFriction;
    }

    void Rigidbody::SetDynamicFriction(const float dynamicFriction)
    {
        if (dynamicFriction < 0.0f) return;
        
        m_ColliderCollection[0]->PhysicalMaterial.DynamicFriction = dynamicFriction;
        
        if (m_Actor != NULL)
        {
            const auto colliders = GetColliders();
            const auto physicalMaterials = GetPhysicalMaterials(colliders);
            
            physicalMaterials[0].setDynamicFriction(dynamicFriction);
        }
    }
    
    float Rigidbody::GetRestitution() const
    {
        return m_ColliderCollection[0]->PhysicalMaterial.Restitution;
    }

    void Rigidbody::SetRestitution(const float restitution)
    {
        if (restitution < 0.0f || restitution > 1.0f) return;
        
        m_ColliderCollection[0]->PhysicalMaterial.Restitution = restitution;
        
        if (m_Actor != NULL)
        {
            const auto colliders = GetColliders();
            const auto physicalMaterials = GetPhysicalMaterials(colliders);
            
            physicalMaterials[0].setRestitution(restitution);
        }
    }
    
    class RigidbodySerializer final : public ComponentSerializer<Rigidbody, RigidbodySerializer>
    {
    protected:
        const char* GetKey() override { return "Rigidbody"; }

        void SerializeData(YAML::Emitter& out, const Rigidbody& component) override
        {
            out << YAML::Key << "Visualization Flag" << YAML::Value << component.IsVisualizationEnabled();
            out << YAML::Key << "Rigidbody Type" << YAML::Value << static_cast<int>(component.GetRigidbodyType());
            out << YAML::Key << "Gravity Flag" << YAML::Value << component.IsGravityEnabled();
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
            
            out << YAML::Key << "Static Friction" << YAML::Value << component.GetStaticFriction();
            out << YAML::Key << "Dynamic Friction" << YAML::Value << component.GetDynamicFriction();
            out << YAML::Key << "Restitution" << YAML::Value << component.GetRestitution(); 
        }

        void DeserializeData(YAML::Node& node, Rigidbody& component) override
        {
            if (const auto colliderVisualizationFlagNode = node["Visualization Flag"])
            {
                component.EnableVisualization(colliderVisualizationFlagNode.as<bool>());
            }
            
            if (const auto rigidbodyTypeNode = node["Rigidbody Type"])
            {
                component.SetRigidbodyType(static_cast<Rigidbody::Type>(rigidbodyTypeNode.as<int>()));
            }

            if (const auto rigidbodyGravityFlagNode = node["Gravity Flag"])
            {
                component.EnableGravity(rigidbodyGravityFlagNode.as<bool>());
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
