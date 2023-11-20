#include "levpch.h"
#include "PhysicsRigidbody.h"
#include "PhysicsBase.h"
#include "PhysicsUtils.h"
#include "Scene/Components/ComponentSerializer.h"

constexpr auto MAX_COLLIDER_NUMBER = 1;
constexpr auto MAX_MATERIAL_NUMBER = 1;

constexpr auto DEFAULT_RIGIDBODY_DENSITY = 10;
constexpr auto DEFAULT_VISUALIZATION_FLAG = true;
constexpr auto DEFAULT_RIGIDBODY_TYPE = RigidbodyType::Dynamic;
constexpr auto DEFAULT_GRAVITY_FLAG = true;
constexpr auto DEFAULT_COLLIDER_TYPE = ColliderType::Box;
constexpr auto DEFAULT_COLLIDER_SIZE = 0.5f;
constexpr auto DEFAULT_STATIC_FRICTION = 0.5f;
constexpr auto DEFAULT_DYNAMIC_FRICTION = 0.5f;
constexpr auto DEFAULT_RESTITUTION = 0.6f;

namespace LevEngine
{
    void PhysicsRigidbody::OnDestroy(entt::registry& registry, entt::entity entity)
    {
        auto& rigidbody = registry.get<PhysicsRigidbody>(entity);
        rigidbody.DetachRigidbody();
    }

    

    PBaseRigidbody* PhysicsRigidbody::GetRigidbody() const
    {
        return m_Rigidbody;
    }

    
    
    PCollider* PhysicsRigidbody::GetColliders() const
    {
        PCollider* colliders[MAX_COLLIDER_NUMBER];
        const auto nbColliders = m_Rigidbody->getNbShapes();
        m_Rigidbody->getShapes(colliders, nbColliders);

        return *colliders;
    }

    PPhysicalMaterial* PhysicsRigidbody::GetPhysicalMaterials(const PCollider* colliders) const
    {
        PPhysicalMaterial* physicalMaterials[MAX_MATERIAL_NUMBER];
        const auto nbMaterials = colliders[0].getNbMaterials();
        colliders[0].getMaterials(physicalMaterials, nbMaterials);

        return *physicalMaterials;
    }

    
    
    bool PhysicsRigidbody::GetInitializationFlag() const
    {
        return m_IsInitialized;
    }
    
    void PhysicsRigidbody::Initialize(const Transform& transform)
    {
        if (m_IsInitialized) return;
        
        AttachRigidbody(GetRigidbodyType());
        SetRigidbodyPose(transform);
        SetColliderVisualizationFlag(GetColliderVisualizationFlag());
        SetRigidbodyGravityFlag(GetRigidbodyGravityFlag());
        AttachCollider(GetColliderType());
        SetColliderOffsetPosition(GetColliderOffsetPosition());
        SetColliderOffsetRotation(GetColliderOffsetRotation());

        m_IsInitialized = true;
    }

    void PhysicsRigidbody::SetRigidbodyPose(const Transform& transform)
    {
        const PxTransform pxTransform = PhysicsUtils::FromTransformToPxTransform(transform);
        m_Rigidbody->setGlobalPose(pxTransform);
    }


    
    RigidbodyType PhysicsRigidbody::GetRigidbodyType() const
    {
        return m_RigidbodyType;
    }

    void PhysicsRigidbody::SetRigidbodyType(const RigidbodyType& rigidbodyType)
    {
        m_RigidbodyType = rigidbodyType;
    }

    void PhysicsRigidbody::AttachRigidbody(const RigidbodyType& rigidbodyType)
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
            m_Rigidbody = PhysicsBase::GetInstance().GetFactory()->createRigidStatic(initialPose);
            PhysicsBase::GetInstance().GetCollection()->addActor(*(reinterpret_cast<PStaticRigidbody*>(m_Rigidbody)));
            SetRigidbodyGravityFlag(!DEFAULT_GRAVITY_FLAG);
            break;
        case RigidbodyType::Dynamic:
            m_Rigidbody = PhysicsBase::GetInstance().GetFactory()->createRigidDynamic(initialPose);
            PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<PDynamicRigidbody*>(m_Rigidbody)), DEFAULT_RIGIDBODY_DENSITY);
            PhysicsBase::GetInstance().GetCollection()->addActor(*(reinterpret_cast<PDynamicRigidbody*>(m_Rigidbody)));
            //SetRigidbodyGravityFlag(DEFAULT_GRAVITY_FLAG);
            break;
        default:
            break;
        }
    }

    void PhysicsRigidbody::DetachRigidbody()
    {
        DetachCollider();

        if (m_Rigidbody != NULL)
        {
            m_Rigidbody->getScene()->removeActor(*m_Rigidbody);
            PX_RELEASE(m_Rigidbody); 
        }
    }
    
    bool PhysicsRigidbody::GetRigidbodyGravityFlag() const
    {
        return m_IsRigidbodyGravityEnabled;
    }
    
    void PhysicsRigidbody::SetRigidbodyGravityFlag(const bool flag)
    {
        m_IsRigidbodyGravityEnabled = flag;
        
        if (m_Rigidbody != NULL)
        {
            m_Rigidbody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !flag);
        }
    }
    
    
    
    ColliderType PhysicsRigidbody::GetColliderType() const
    {
        return m_ColliderType;
    }

    void PhysicsRigidbody::SetColliderType(const ColliderType& colliderType)
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
            SetSphereRadius(DEFAULT_COLLIDER_SIZE);
            break;
        case ColliderType::Capsule:
            m_ColliderCollection.push_back(CreateRef<Capsule>());
            SetCapsuleRadius(DEFAULT_COLLIDER_SIZE);
            SetCapsuleHalfHeight(DEFAULT_COLLIDER_SIZE);
            break;
        case ColliderType::Box:
            m_ColliderCollection.push_back(CreateRef<Box>());
            SetBoxHalfExtents(Vector3(DEFAULT_COLLIDER_SIZE,DEFAULT_COLLIDER_SIZE,DEFAULT_COLLIDER_SIZE));
            break;
        default:
            break;
        }

        SetColliderOffsetPosition(Vector3::Zero);
        SetColliderOffsetRotation(Vector3::Zero);

        m_ColliderCollection[0]->MaterialCollection.push_back(PhysicalMaterial());
        SetStaticFriction(DEFAULT_STATIC_FRICTION);
        SetDynamicFriction(DEFAULT_DYNAMIC_FRICTION);
        SetRestitution(DEFAULT_RESTITUTION);
    }

    void PhysicsRigidbody::AttachCollider(const ColliderType& colliderType)
    {
        const auto physicalMaterial = PhysicsBase::GetInstance().GetFactory()->createMaterial(GetStaticFriction(), GetDynamicFriction(), GetRestitution());
        
        PCollider* collider = nullptr;
        switch (colliderType)
        {
        case ColliderType::Sphere:
            collider = PhysicsBase::GetInstance().GetFactory()->createShape(PxSphereGeometry(GetSphereRadius()), *physicalMaterial, true);
            break;
        case ColliderType::Capsule:
            collider = PhysicsBase::GetInstance().GetFactory()->createShape(PxCapsuleGeometry(GetCapsuleRadius(), GetCapsuleHalfHeight()), *physicalMaterial, true);
            break;
        case ColliderType::Box:
            collider = PhysicsBase::GetInstance().GetFactory()->createShape(PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(GetBoxHalfExtents())), *physicalMaterial, true);
            break;
        default:
            break;
        }
    
        m_Rigidbody->attachShape(*collider);
        collider->release();
        physicalMaterial->release();  
    }
    
    void PhysicsRigidbody::DetachCollider()
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

    int PhysicsRigidbody::GetColliderNumber() const
    {
        return m_ColliderCollection.size();
    }

    bool PhysicsRigidbody::GetColliderVisualizationFlag() const
    {
        return m_IsColliderVisualizationEnabled;
    }
    
    void PhysicsRigidbody::SetColliderVisualizationFlag(const bool flag)
    {
        m_IsColliderVisualizationEnabled = flag;
        
        if (m_Rigidbody != NULL)
        {
            m_Rigidbody->setActorFlag(PxActorFlag::eVISUALIZATION, flag);
        }
    }
    
    
    
    Vector3 PhysicsRigidbody::GetColliderOffsetPosition() const
    {
        return m_ColliderCollection[0]->OffsetPosition;
    }

    void PhysicsRigidbody::SetColliderOffsetPosition(const Vector3 position)
    {
        m_ColliderCollection[0]->OffsetPosition = position;
        
        if (m_Rigidbody != NULL)
        {
            const auto colliders = GetColliders();
            const auto rotation = colliders[0].getLocalPose().q;
            colliders[0].setLocalPose(PxTransform(PhysicsUtils::FromVector3ToPxVec3(position), rotation));
        }
    }

    Vector3 PhysicsRigidbody::GetColliderOffsetRotation() const
    {
        return m_ColliderCollection[0]->OffsetRotation;
    }

    void PhysicsRigidbody::SetColliderOffsetRotation(const Vector3 rotation)
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


    
    float PhysicsRigidbody::GetSphereRadius() const
    {
        return static_cast<Sphere*>(m_ColliderCollection[0].get())->Radius;
    }

    void PhysicsRigidbody::SetSphereRadius(const float radius)
    {
        if (radius < 0.0f || radius > FLT_MAX) return;
        
        static_cast<Sphere*>(m_ColliderCollection[0].get())->Radius = radius;
        
        if (m_Rigidbody != NULL)
        {
            const auto colliders = GetColliders();
            colliders[0].setGeometry(PxSphereGeometry(radius));
        }
    }

    float PhysicsRigidbody::GetCapsuleRadius() const
    {
        return static_cast<Capsule*>(m_ColliderCollection[0].get())->Radius;
    }

    void PhysicsRigidbody::SetCapsuleRadius(const float radius)
    {
        if (radius < 0.0f || radius > FLT_MAX) return;
        
        static_cast<Capsule*>(m_ColliderCollection[0].get())->Radius = radius;
        
        if (m_Rigidbody != NULL)
        {
            const auto colliders = GetColliders();
            const PxGeometryHolder geom(colliders[0].getGeometry());
            const auto initialHeight = geom.capsule().halfHeight;
            colliders[0].setGeometry(PxCapsuleGeometry(radius, initialHeight));
        }
    }
    
    float PhysicsRigidbody::GetCapsuleHalfHeight() const
    {
        return static_cast<Capsule*>(m_ColliderCollection[0].get())->HalfHeight;
    }

    void PhysicsRigidbody::SetCapsuleHalfHeight(const float halfHeight)
    {
        if (halfHeight < 0.0f || halfHeight > FLT_MAX) return;
        
        static_cast<Capsule*>(m_ColliderCollection[0].get())->HalfHeight = halfHeight;
        
        if (m_Rigidbody != NULL)
        {
            const auto colliders = GetColliders();
            const PxGeometryHolder geom(colliders[0].getGeometry());
            const auto initialRadius = geom.capsule().radius;
            colliders[0].setGeometry(PxCapsuleGeometry(initialRadius, halfHeight));
        }
    }

    Vector3 PhysicsRigidbody::GetBoxHalfExtents() const
    {
        return static_cast<Box*>(m_ColliderCollection[0].get())->HalfExtents;
    }

    void PhysicsRigidbody::SetBoxHalfExtents(const Vector3 halfExtents)
    {
        if ((halfExtents.x < 0.0f || halfExtents.x > FLT_MAX) || (halfExtents.y < 0.0f || halfExtents.y > FLT_MAX) || (halfExtents.z < 0.0f || halfExtents.z > FLT_MAX)) return;
        
        static_cast<Box*>(m_ColliderCollection[0].get())->HalfExtents = halfExtents;
        
        if (m_Rigidbody != NULL)
        {
            const auto colliders = GetColliders();
            colliders[0].setGeometry(PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(halfExtents)));
        }
    }
    
    
    
    float PhysicsRigidbody::GetStaticFriction() const
    {
        return m_ColliderCollection[0]->MaterialCollection[0].StaticFriction;
    }

    void PhysicsRigidbody::SetStaticFriction(const float staticFriction)
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
    
    float PhysicsRigidbody::GetDynamicFriction() const
    {
        return m_ColliderCollection[0]->MaterialCollection[0].DynamicFriction;
    }

    void PhysicsRigidbody::SetDynamicFriction(const float dynamicFriction)
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
    
    float PhysicsRigidbody::GetRestitution() const
    {
        return m_ColliderCollection[0]->MaterialCollection[0].Restitution;
    }

    void PhysicsRigidbody::SetRestitution(const float restitution)
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
    
    class PhysicsRigidbodySerializer final : public ComponentSerializer<PhysicsRigidbody, PhysicsRigidbodySerializer>
    {
    protected:
        const char* GetKey() override { return "Physics Rigidbody"; }

        void SerializeData(YAML::Emitter& out, const PhysicsRigidbody& component) override
        {
            out << YAML::Key << "Visualization Flag" << YAML::Value << component.GetColliderVisualizationFlag();
            out << YAML::Key << "Rigidbody Type" << YAML::Value << static_cast<int>(component.GetRigidbodyType());
            out << YAML::Key << "Gravity Flag" << YAML::Value << component.GetRigidbodyGravityFlag();
            out << YAML::Key << "Collider Type" << YAML::Value << static_cast<int>(component.GetColliderType());

            switch (component.GetColliderType())
            {
            case ColliderType::Sphere:
                out << YAML::Key << "Sphere Radius" << YAML::Value << component.GetSphereRadius();
                break;
            case ColliderType::Capsule:
                out << YAML::Key << "Capsule Radius" << YAML::Value << component.GetCapsuleRadius();
                out << YAML::Key << "Capsule Half Height" << YAML::Value << component.GetCapsuleHalfHeight();
                break;
            case ColliderType::Box:
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

        void DeserializeData(YAML::Node& node, PhysicsRigidbody& component) override
        {
            if (const auto colliderVisualizationFlagNode = node["Visualization Flag"])
            {
                component.SetColliderVisualizationFlag(colliderVisualizationFlagNode.as<bool>());
            }
            else
            {
                component.SetColliderVisualizationFlag(DEFAULT_VISUALIZATION_FLAG);
            }
            
            if (const auto rigidbodyTypeNode = node["Rigidbody Type"])
            {
                component.SetRigidbodyType(static_cast<RigidbodyType>(rigidbodyTypeNode.as<int>()));
            }
            else
            {
                component.SetRigidbodyType(DEFAULT_RIGIDBODY_TYPE);
            }

            if (const auto rigidbodyGravityFlagNode = node["Gravity Flag"])
            {
                component.SetRigidbodyGravityFlag(rigidbodyGravityFlagNode.as<bool>());
            }
            else
            {
                component.SetRigidbodyGravityFlag(DEFAULT_GRAVITY_FLAG);
            }

            if (const auto colliderTypeNode = node["Collider Type"])
            {
                switch (const auto colliderType = static_cast<ColliderType>(colliderTypeNode.as<int>()))
                {
                case ColliderType::Sphere:
                    component.SetColliderType(colliderType);
                    if (const auto sphereRadiusNode = node["Sphere Radius"])
                    {
                        component.SetSphereRadius(sphereRadiusNode.as<float>());
                    }
                    else
                    {
                        component.SetSphereRadius(DEFAULT_COLLIDER_SIZE);
                    }
                    break;
                case ColliderType::Capsule:
                    component.SetColliderType(colliderType);
                    if (const auto capsuleRadiusNode = node["Capsule Radius"])
                    {
                        component.SetCapsuleRadius(capsuleRadiusNode.as<float>());
                    }
                    else
                    {
                        component.SetCapsuleRadius(DEFAULT_COLLIDER_SIZE);
                    }
                    if (const auto capsuleHalfHeightNode = node["Capsule Half Height"])
                    {
                        component.SetCapsuleHalfHeight(capsuleHalfHeightNode.as<float>());
                    }
                    else
                    {
                        component.SetCapsuleHalfHeight(DEFAULT_COLLIDER_SIZE);
                    }
                    break;
                case ColliderType::Box:
                    component.SetColliderType(colliderType);
                    if (const auto boxExtents = node["Box Half Extends"])
                    {
                        component.SetBoxHalfExtents(boxExtents.as<Vector3>());
                    }
                    else
                    {
                        component.SetBoxHalfExtents(Vector3(DEFAULT_COLLIDER_SIZE,DEFAULT_COLLIDER_SIZE,DEFAULT_COLLIDER_SIZE));
                    }
                    break;
                default:
                    break;
                }
            }
            else
            {
                component.SetColliderType(DEFAULT_COLLIDER_TYPE);
                component.SetBoxHalfExtents(Vector3(DEFAULT_COLLIDER_SIZE,DEFAULT_COLLIDER_SIZE,DEFAULT_COLLIDER_SIZE));
            }

            if (const auto colliderOffsetPositionNode = node["Offset Position"])
            {
                component.SetColliderOffsetPosition(colliderOffsetPositionNode.as<Vector3>());
            }
            else
            {
                component.SetColliderOffsetPosition(Vector3::Zero);
            }

            if (const auto collideOffsetRotationNode = node["Offset Rotation"])
            {
                component.SetColliderOffsetRotation(collideOffsetRotationNode.as<Vector3>());
            }
            else
            {
                component.SetColliderOffsetRotation(Vector3::Zero);
            }

            if (const auto staticFrictionNode = node["Static Friction"])
            {
                component.SetStaticFriction(staticFrictionNode.as<float>());
            }
            else
            {
                component.SetStaticFriction(DEFAULT_STATIC_FRICTION);
            }

            if (const auto dynamicFrictionNode = node["Dynamic Friction"])
            {
                component.SetDynamicFriction(dynamicFrictionNode.as<float>());
            }
            else
            {
                component.SetDynamicFriction(DEFAULT_DYNAMIC_FRICTION);
            }

            if (const auto restitutionNode = node["Restitution"])
            {
                component.SetRestitution(restitutionNode.as<float>());
            }
            else
            {
                component.SetRestitution(DEFAULT_RESTITUTION);
            }
        } 
    };
}
