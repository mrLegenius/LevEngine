#include "levpch.h"
#include "Rigidbody.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsUtils.h"
#include "Scene/Components/ComponentSerializer.h"

constexpr auto MAX_COLLIDER_NUMBER = 1;
constexpr auto MAX_MATERIAL_NUMBER = 1;

constexpr auto DEFAULT_RIGIDBODY_DENSITY = 10;
constexpr auto DEFAULT_VISUALIZATION_FLAG = true;
constexpr auto DEFAULT_RIGIDBODY_TYPE = Rigidbody::RigidbodyType::Dynamic;
constexpr auto DEFAULT_GRAVITY_FLAG = true;
constexpr auto DEFAULT_COLLIDER_TYPE = Rigidbody::ColliderType::Box;
constexpr auto DEFAULT_COLLIDER_SIZE = 0.5f;
constexpr auto DEFAULT_STATIC_FRICTION = 0.5f;
constexpr auto DEFAULT_DYNAMIC_FRICTION = 0.5f;
constexpr auto DEFAULT_RESTITUTION = 0.6f;

namespace LevEngine
{
    void Rigidbody::OnDestroy(entt::registry& registry, entt::entity entity)
    {
        auto& rigidbody = registry.get<Rigidbody>(entity);
        rigidbody.DetachRigidbody();
    }

    

    PxRigidActor* Rigidbody::GetRigidbody() const
    {
        return m_Rigidbody;
    }
    
    PxShape* Rigidbody::GetColliders() const
    {
        PxShape* colliders[MAX_COLLIDER_NUMBER];
        const auto nbColliders = m_Rigidbody->getNbShapes();
        m_Rigidbody->getShapes(colliders, nbColliders);

        return *colliders;
    }

    PxMaterial* Rigidbody::GetPhysicalMaterials(const PxShape* colliders) const
    {
        PxMaterial* physicalMaterials[MAX_MATERIAL_NUMBER];
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
        m_TransformScale = transformScale;
        switch(m_ColliderType)
        {
        case ColliderType::Sphere:
            SetSphereRadius(GetSphereRadius());
            break;
        case ColliderType::Capsule:
            SetCapsuleRadius(GetCapsuleRadius());
            SetCapsuleHalfHeight(GetCapsuleHalfHeight());
            break;
        case ColliderType::Box:
            SetBoxHalfExtents(GetBoxHalfExtents());
            break;
        default:
            break;
        }
    }

    void Rigidbody::ApplyTransformScale()
    {
        switch(m_ColliderType)
        {
        case ColliderType::Sphere:
            SetSphereRadius(GetSphereRadius());
            break;
        case ColliderType::Capsule:
            SetCapsuleRadius(GetCapsuleRadius());
            SetCapsuleHalfHeight(GetCapsuleHalfHeight());
            break;
        case ColliderType::Box:
            SetBoxHalfExtents(GetBoxHalfExtents());
            break;
        default:
            break;
        }
    }

    
    
    bool Rigidbody::GetInitializationFlag() const
    {
        return m_IsInitialized;
    }
    
    void Rigidbody::Initialize(const Transform& transform)
    {
        if (m_IsInitialized) return;
        
        AttachRigidbody(m_RigidbodyType);
        SetRigidbodyPose(transform);
        SetColliderVisualizationFlag(m_IsColliderVisualizationEnabled);
        SetRigidbodyGravityFlag(m_IsRigidbodyGravityEnabled);
        AttachCollider(m_ColliderType);
        SetColliderOffsetPosition(GetColliderOffsetPosition());
        SetColliderOffsetRotation(GetColliderOffsetRotation());
        
        SetTransformScale(transform.GetWorldScale());
        ApplyTransformScale();

        m_IsInitialized = true;
    }

    void Rigidbody::SetRigidbodyPose(const Transform& transform)
    {
        const PxTransform pxTransform = PhysicsUtils::FromTransformToPxTransform(transform);
        m_Rigidbody->setGlobalPose(pxTransform);
    }


    
    Rigidbody::RigidbodyType Rigidbody::GetRigidbodyType() const
    {
        return m_RigidbodyType;
    }

    void Rigidbody::SetRigidbodyType(const RigidbodyType& rigidbodyType)
    {
        m_RigidbodyType = rigidbodyType;
    }

    void Rigidbody::AttachRigidbody(const RigidbodyType& rigidbodyType)
    {
        auto initialPose = PxTransform(PxIdentity);
        if (m_Rigidbody != NULL)
        {
            initialPose = m_Rigidbody->getGlobalPose();
            DetachRigidbody();
        }
    
        switch (rigidbodyType)
        {
        case RigidbodyType::Static:
            m_Rigidbody = Physics::GetInstance().GetPhysics()->createRigidStatic(initialPose);
            Physics::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidStatic*>(m_Rigidbody)));
            SetRigidbodyGravityFlag(!DEFAULT_GRAVITY_FLAG);
            break;
        case RigidbodyType::Dynamic:
            m_Rigidbody = Physics::GetInstance().GetPhysics()->createRigidDynamic(initialPose);
            PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<PxRigidDynamic*>(m_Rigidbody)), DEFAULT_RIGIDBODY_DENSITY);
            Physics::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidDynamic*>(m_Rigidbody)));
            SetRigidbodyGravityFlag(GetRigidbodyGravityFlag());
            break;
        default:
            break;
        }
    }

    void Rigidbody::DetachRigidbody()
    {
        DetachCollider();

        if (m_Rigidbody != NULL)
        {
            m_Rigidbody->getScene()->removeActor(*m_Rigidbody);
            PX_RELEASE(m_Rigidbody); 
        }
    }
    
    bool Rigidbody::GetRigidbodyGravityFlag() const
    {
        return m_IsRigidbodyGravityEnabled;
    }
    
    void Rigidbody::SetRigidbodyGravityFlag(const bool flag)
    {
        m_IsRigidbodyGravityEnabled = flag;
        
        if (m_Rigidbody != NULL)
        {
            m_Rigidbody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !flag);
        }
    }
    
    
    
    Rigidbody::ColliderType Rigidbody::GetColliderType() const
    {
        return m_ColliderType;
    }

    void Rigidbody::SetColliderType(const ColliderType& colliderType)
    {
        m_ColliderType = colliderType;

        if (GetColliderNumber() >= MAX_COLLIDER_NUMBER)
        {
            DetachCollider();
        }
        
        switch (colliderType)
        {
        case ColliderType::Sphere:
            m_ColliderCollection.push_back(CreateRef<Sphere>());
            break;
        case ColliderType::Capsule:
            m_ColliderCollection.push_back(CreateRef<Capsule>());
            break;
        case ColliderType::Box:
            m_ColliderCollection.push_back(CreateRef<Box>());
            break;
        default:
            break;
        }

        m_ColliderCollection[0]->MaterialCollection.push_back(PhysicalMaterial());
    }

    void Rigidbody::AttachCollider(const ColliderType& colliderType)
    {
        const auto physicalMaterial = Physics::GetInstance().GetPhysics()->createMaterial(GetStaticFriction(), GetDynamicFriction(), GetRestitution());
        
        PxShape* collider = nullptr;
        switch (colliderType)
        {
        case ColliderType::Sphere:
            collider = Physics::GetInstance().GetPhysics()->createShape(PxSphereGeometry(GetSphereRadius()), *physicalMaterial, true);
            break;
        case ColliderType::Capsule:
            collider = Physics::GetInstance().GetPhysics()->createShape(PxCapsuleGeometry(GetCapsuleRadius(), GetCapsuleHalfHeight()), *physicalMaterial, true);
            break;
        case ColliderType::Box:
            collider = Physics::GetInstance().GetPhysics()->createShape(PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(GetBoxHalfExtents())), *physicalMaterial, true);
            break;
        default:
            break;
        }
    
        m_Rigidbody->attachShape(*collider);
        collider->release();
        physicalMaterial->release();  
    }
    
    void Rigidbody::DetachCollider()
    {
        if (GetColliderNumber() <= 0) return;

        if (m_Rigidbody != NULL)
        {
            const auto colliders = GetColliders();
            for (auto i = 0; i < GetColliderNumber(); i++)
            {
                m_Rigidbody->detachShape(colliders[i]);
            }
        }
        
        m_ColliderCollection[0]->MaterialCollection.clear();
        m_ColliderCollection.clear();
    }

    int Rigidbody::GetColliderNumber() const
    {
        return m_ColliderCollection.size();
    }

    bool Rigidbody::GetColliderVisualizationFlag() const
    {
        return m_IsColliderVisualizationEnabled;
    }
    
    void Rigidbody::SetColliderVisualizationFlag(const bool flag)
    {
        m_IsColliderVisualizationEnabled = flag;
        
        if (m_Rigidbody != NULL)
        {
            m_Rigidbody->setActorFlag(PxActorFlag::eVISUALIZATION, flag);
        }
    }
    
    
    
    Vector3 Rigidbody::GetColliderOffsetPosition() const
    {
        return m_ColliderCollection[0]->OffsetPosition;
    }

    void Rigidbody::SetColliderOffsetPosition(const Vector3 position)
    {
        m_ColliderCollection[0]->OffsetPosition = position;
        
        if (m_Rigidbody != NULL)
        {
            const auto colliders = GetColliders();
            const auto rotation = colliders[0].getLocalPose().q;
            colliders[0].setLocalPose(PxTransform(PhysicsUtils::FromVector3ToPxVec3(position), rotation));
        }
    }

    Vector3 Rigidbody::GetColliderOffsetRotation() const
    {
        return m_ColliderCollection[0]->OffsetRotation;
    }

    void Rigidbody::SetColliderOffsetRotation(const Vector3 rotation)
    {
        m_ColliderCollection[0]->OffsetRotation = rotation;
        
        if (m_Rigidbody != NULL)
        {
            const auto colliders = GetColliders();
            const auto position = colliders[0].getLocalPose().p;
            const auto quaternion = Quaternion::CreateFromYawPitchRoll(Math::ToRadians(rotation));
            colliders[0].setLocalPose(PxTransform(position, PhysicsUtils::FromQuaternionToPxQuat(quaternion)));
        }
    }


    
    float Rigidbody::GetSphereRadius() const
    {
        return static_cast<Sphere*>(m_ColliderCollection[0].get())->Radius;
    }

    void Rigidbody::SetSphereRadius(const float radius)
    {
        if (radius <= 0.0f || radius >= FLT_MAX) return;
        
        static_cast<Sphere*>(m_ColliderCollection[0].get())->Radius = radius;
        
        if (m_Rigidbody != NULL)
        {
            const float maxTransformScale = std::max(m_TransformScale.x, std::max(m_TransformScale.y, m_TransformScale.z));
            
            const auto colliders = GetColliders();
            colliders[0].setGeometry(PxSphereGeometry(radius * maxTransformScale));
        }
    }

    float Rigidbody::GetCapsuleRadius() const
    {
        return static_cast<Capsule*>(m_ColliderCollection[0].get())->Radius;
    }

    void Rigidbody::SetCapsuleRadius(const float radius)
    {
        if (radius <= 0.0f || radius >= FLT_MAX) return;
        
        static_cast<Capsule*>(m_ColliderCollection[0].get())->Radius = radius;
        
        if (m_Rigidbody != NULL)
        {
            const float maxTransformScale = std::max(m_TransformScale.x, std::max(m_TransformScale.y, m_TransformScale.z));
            
            const auto colliders = GetColliders();
            const PxGeometryHolder geom(colliders[0].getGeometry());
            const auto initialHeight = geom.capsule().halfHeight;
            colliders[0].setGeometry(PxCapsuleGeometry(radius * maxTransformScale, initialHeight));
        }
    }
    
    float Rigidbody::GetCapsuleHalfHeight() const
    {
        return static_cast<Capsule*>(m_ColliderCollection[0].get())->HalfHeight;
    }

    void Rigidbody::SetCapsuleHalfHeight(const float halfHeight)
    {
        if (halfHeight < 0.0f || halfHeight >= FLT_MAX) return;
        
        static_cast<Capsule*>(m_ColliderCollection[0].get())->HalfHeight = halfHeight;
        
        if (m_Rigidbody != NULL)
        {
            const float maxTransformScale = std::max(m_TransformScale.x, std::max(m_TransformScale.y, m_TransformScale.z));
            
            const auto colliders = GetColliders();
            const PxGeometryHolder geom(colliders[0].getGeometry());
            const auto initialRadius = geom.capsule().radius;
            colliders[0].setGeometry(PxCapsuleGeometry(initialRadius, halfHeight * maxTransformScale));
        }
    }

    Vector3 Rigidbody::GetBoxHalfExtents() const
    {
        return static_cast<Box*>(m_ColliderCollection[0].get())->HalfExtents;
    }

    void Rigidbody::SetBoxHalfExtents(const Vector3 halfExtents)
    {
        if ((halfExtents.x <= 0.0f || halfExtents.x >= FLT_MAX) || (halfExtents.y <= 0.0f || halfExtents.y >= FLT_MAX) || (halfExtents.z <= 0.0f || halfExtents.z >= FLT_MAX)) return;
        
        static_cast<Box*>(m_ColliderCollection[0].get())->HalfExtents = halfExtents;
        
        if (m_Rigidbody != NULL)
        {
            const auto colliders = GetColliders();
            colliders[0].setGeometry(PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(halfExtents * m_TransformScale)));
        }
    }
    
    
    
    float Rigidbody::GetStaticFriction() const
    {
        return m_ColliderCollection[0]->MaterialCollection[0].StaticFriction;
    }

    void Rigidbody::SetStaticFriction(const float staticFriction)
    {
        if (staticFriction < 0.0f || staticFriction > FLT_MAX) return;
        
        m_ColliderCollection[0]->MaterialCollection[0].StaticFriction = staticFriction;
        
        if (m_Rigidbody != NULL)
        {
            const auto colliders = GetColliders();
            const auto physicalMaterials = GetPhysicalMaterials(colliders);
            
            physicalMaterials[0].setStaticFriction(staticFriction);
        }
    }
    
    float Rigidbody::GetDynamicFriction() const
    {
        return m_ColliderCollection[0]->MaterialCollection[0].DynamicFriction;
    }

    void Rigidbody::SetDynamicFriction(const float dynamicFriction)
    {
        if (dynamicFriction < 0.0f || dynamicFriction > FLT_MAX) return;
        
        m_ColliderCollection[0]->MaterialCollection[0].DynamicFriction = dynamicFriction;
        
        if (m_Rigidbody != NULL)
        {
            const auto colliders = GetColliders();
            const auto physicalMaterials = GetPhysicalMaterials(colliders);
            
            physicalMaterials[0].setDynamicFriction(dynamicFriction);
        }
    }
    
    float Rigidbody::GetRestitution() const
    {
        return m_ColliderCollection[0]->MaterialCollection[0].Restitution;
    }

    void Rigidbody::SetRestitution(const float restitution)
    {
        if (restitution < 0.0f || restitution > 1.0f) return;
        
        m_ColliderCollection[0]->MaterialCollection[0].Restitution = restitution;
        
        if (m_Rigidbody != NULL)
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
            out << YAML::Key << "Visualization Flag" << YAML::Value << component.GetColliderVisualizationFlag();
            out << YAML::Key << "Rigidbody Type" << YAML::Value << static_cast<int>(component.GetRigidbodyType());
            out << YAML::Key << "Gravity Flag" << YAML::Value << component.GetRigidbodyGravityFlag();
            out << YAML::Key << "Collider Type" << YAML::Value << static_cast<int>(component.GetColliderType());

            switch (component.GetColliderType())
            {
            case Rigidbody::ColliderType::Sphere:
                out << YAML::Key << "Sphere Radius" << YAML::Value << component.GetSphereRadius();
                break;
            case Rigidbody::ColliderType::Capsule:
                out << YAML::Key << "Capsule Radius" << YAML::Value << component.GetCapsuleRadius();
                out << YAML::Key << "Capsule Half Height" << YAML::Value << component.GetCapsuleHalfHeight();
                break;
            case Rigidbody::ColliderType::Box:
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
                component.SetColliderVisualizationFlag(colliderVisualizationFlagNode.as<bool>());
            }
            
            if (const auto rigidbodyTypeNode = node["Rigidbody Type"])
            {
                component.SetRigidbodyType(static_cast<Rigidbody::RigidbodyType>(rigidbodyTypeNode.as<int>()));
            }

            if (const auto rigidbodyGravityFlagNode = node["Gravity Flag"])
            {
                component.SetRigidbodyGravityFlag(rigidbodyGravityFlagNode.as<bool>());
            }

            if (const auto colliderTypeNode = node["Collider Type"])
            {
                switch (const auto colliderType = static_cast<Rigidbody::ColliderType>(colliderTypeNode.as<int>()))
                {
                case Rigidbody::ColliderType::Sphere:
                    component.SetColliderType(colliderType);
                    if (const auto sphereRadiusNode = node["Sphere Radius"])
                    {
                        component.SetSphereRadius(sphereRadiusNode.as<float>());
                    }
                    break;
                case Rigidbody::ColliderType::Capsule:
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
                case Rigidbody::ColliderType::Box:
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
