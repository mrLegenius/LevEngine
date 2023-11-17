#include "levpch.h"
#include "PhysicsRigidbody.h"
#include "PhysicsBase.h"
#include "PhysicsUtils.h"
#include "Scene/Components/ComponentSerializer.h"

constexpr auto DEFAULT_ACTOR_DENSITY = 10;
constexpr auto DEFAULT_STATIC_FRICTION = 0.5f;
constexpr auto DEFAULT_DYNAMIC_FRICTION = 0.5f;
constexpr auto DEFAULT_RESTITUTION = 0.6f;
constexpr auto DEFAULT_SHAPE_SIZE = 0.5f;

namespace LevEngine
{
    PhysicsRigidbody::PhysicsRigidbody()
    {
        m_Actor = PhysicsBase::GetInstance().GetFactory()->createRigidDynamic(PxTransform(PxIdentity));
        PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<DynamicActor*>(m_Actor)), DEFAULT_ACTOR_DENSITY);
        PhysicsBase::GetInstance().GetCollection()->addActor(*(reinterpret_cast<DynamicActor*>(m_Actor)));
        AttachCollider(ColliderType::Box);
    }

    void PhysicsRigidbody::SetPose(const Transform& transform)
    {
        const PxTransform pxTransform = PhysicsUtils::FromTransformToPxTransform(transform);
        m_Actor->setGlobalPose(pxTransform);
    }
    
    void PhysicsRigidbody::Initialize(Transform& transform)
    {
        if (m_IsInitialized) return;
        
        SetPose(transform);
        
        m_IsInitialized = true;
    }

    void PhysicsRigidbody::Deinitialize()
    {
        m_IsInitialized = false;
    }

    bool PhysicsRigidbody::GetInitializationFlag() const
    {
        return m_IsInitialized;
    }
    
    void PhysicsRigidbody::OnDestroy(entt::registry& registry, entt::entity entity)
    {
        auto& rigidbody = registry.get<PhysicsRigidbody>(entity);
        rigidbody.CleanupRigidbody();
    }
    
    void PhysicsRigidbody::CleanupRigidbody()
    {
        const auto colliders = GetColliders();
        
        for(auto i = 0; i < GetColliderNumber(); ++i)
        {
            m_Actor->detachShape(colliders[i]);
        }
        
        m_Actor->getScene()->removeActor(*m_Actor);
        
        PX_RELEASE(m_Actor);
    }

    int PhysicsRigidbody::GetColliderNumber() const
    {
        return m_Actor->getNbShapes();
    }
    
    Collider* PhysicsRigidbody::GetColliders() const
    {
        PxShape* colliders[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbColliders = m_Actor->getNbShapes();
        m_Actor->getShapes(colliders, nbColliders);

        return *colliders;
    }

    PhysicalMaterial* PhysicsRigidbody::GetPhysicalMaterials(const Collider* colliders) const
    {
        PxMaterial* materials[MAX_NUM_RIGIDBODY_MATERIALS];
        const auto nbMaterials = colliders[0].getNbMaterials();
        colliders[0].getMaterials(materials, nbMaterials);

        return *materials;
    }

    
    Vector3 PhysicsRigidbody::GetColliderLocalPosition() const
    {
        const auto colliders = GetColliders();
        
        return PhysicsUtils::FromPxVec3ToVector3(colliders[0].getLocalPose().p);
    }

    void PhysicsRigidbody::SetColliderLocalPosition(const Vector3 position)
    {
        const auto colliders = GetColliders();
        
        const auto rotation = colliders[0].getLocalPose().q;
        
        colliders[0].setLocalPose(PxTransform(PhysicsUtils::FromVector3ToPxVec3(position), rotation));
    }

    Vector3 PhysicsRigidbody::GetColliderLocalRotation() const
    {
        const auto colliders = GetColliders();
        
        const auto quaternion = PhysicsUtils::FromPxQuatToQuaternion(colliders[0].getLocalPose().q);
        
        return Math::ToDegrees(quaternion.ToEuler());
    }

    void PhysicsRigidbody::SetColliderLocalRotation(const Vector3 rotation)
    {
        const auto colliders = GetColliders();
        
        const auto position = colliders[0].getLocalPose().p;
        const auto quaternion = Quaternion::CreateFromYawPitchRoll(Math::ToRadians(rotation));
        
        colliders[0].setLocalPose(PxTransform(position, PhysicsUtils::FromQuaternionToPxQuat(quaternion)));
    }
    
    ColliderType PhysicsRigidbody::GetColliderType() const
    {
        const auto colliders = GetColliders();
        
        if (colliders[0].getGeometry().getType() == PxGeometryType::eSPHERE)
        {
            return ColliderType::Sphere;
        }
        
        if (colliders[0].getGeometry().getType() == PxGeometryType::eCAPSULE)
        {
            return ColliderType::Capsule;
        }
        
        if (colliders[0].getGeometry().getType() == PxGeometryType::eBOX)
        {
            return ColliderType::Box;
        }
    }

    void PhysicsRigidbody::AttachCollider(const ColliderType& colliderType)
    {
        if (GetColliderNumber() > 0)
        {
            if (GetColliderType() == colliderType) return;
            
            if (GetColliderNumber() >= MAX_NUM_RIGIDBODY_SHAPES) return;
            
            DetachCollider();
        }
        
        const auto physicalMaterial = PhysicsBase::GetInstance().GetFactory()->createMaterial(DEFAULT_STATIC_FRICTION, DEFAULT_DYNAMIC_FRICTION, DEFAULT_RESTITUTION);
        PxShape* collider = nullptr;
        switch (colliderType)
        {
        case ColliderType::Sphere:
            collider = PhysicsBase::GetInstance().GetFactory()->createShape(PxSphereGeometry(DEFAULT_SHAPE_SIZE), *physicalMaterial, true);
            break;
        case ColliderType::Capsule:
            collider = PhysicsBase::GetInstance().GetFactory()->createShape(PxCapsuleGeometry(DEFAULT_SHAPE_SIZE, DEFAULT_SHAPE_SIZE), *physicalMaterial, true);
            break;
        case ColliderType::Box:
            collider = PhysicsBase::GetInstance().GetFactory()->createShape(PxBoxGeometry(DEFAULT_SHAPE_SIZE, DEFAULT_SHAPE_SIZE, DEFAULT_SHAPE_SIZE), *physicalMaterial, true);
            break;
        default:
            break;
        }
        
        m_Actor->attachShape(*collider);
        collider->release();
        physicalMaterial->release();
    }
    
    void PhysicsRigidbody::DetachCollider()
    {
        const auto colliders = GetColliders();
        
        for (auto i = 0; i < GetColliderNumber(); i++)
        {
            m_Actor->detachShape(colliders[i]);
        }
    }

    float PhysicsRigidbody::GetSphereColliderRadius() const
    {
        const auto colliders = GetColliders();
        
        const PxGeometryHolder geom(colliders[0].getGeometry());
        const auto radius = geom.sphere().radius;
        
        return radius;
    }

    float PhysicsRigidbody::GetCapsuleColliderRadius() const
    {
        const auto colliders = GetColliders();
        
        const PxGeometryHolder geom(colliders[0].getGeometry());
        const auto radius = geom.capsule().radius;
        
        return radius;
    }
    
    float PhysicsRigidbody::GetCapsuleColliderHalfHeight() const
    {
        const auto colliders = GetColliders();
        
        const PxGeometryHolder geom(colliders[0].getGeometry());
        const auto halfHeight = geom.capsule().halfHeight;
        
        return halfHeight;
    }

    Vector3 PhysicsRigidbody::GetBoxColliderHalfExtents() const
    {
        const auto colliders = GetColliders();
        
        const PxGeometryHolder geom(colliders[0].getGeometry());
        const auto halfExtents = geom.box().halfExtents;

        return PhysicsUtils::FromPxVec3ToVector3(halfExtents);
    }

    
    void PhysicsRigidbody::SetSphereColliderRadius(const float radius)
    {
        if ((radius > 0.0f) && (radius <= PX_MAX_F32))
        {
            const auto colliders = GetColliders();
            
            colliders[0].setGeometry(PxSphereGeometry(radius));
        }
    }
    
    void PhysicsRigidbody::SetCapsuleColliderRadius(const float radius)
    {
        if ((radius > 0.0f) && (radius <= PX_MAX_F32))
        {
            const auto colliders = GetColliders();
            
            const PxGeometryHolder geom(colliders[0].getGeometry());
            const auto initialHeight = geom.capsule().halfHeight;
            
            colliders[0].setGeometry(PxCapsuleGeometry(radius, initialHeight));
        }
    }
    
    void PhysicsRigidbody::SetCapsuleColliderHalfHeight(const float halfHeight)
    {
        if ((halfHeight >= 0.0f) && (halfHeight <= PX_MAX_F32))
        {
            const auto colliders = GetColliders();
            
            const PxGeometryHolder geom(colliders[0].getGeometry());
            const auto initialRadius = geom.capsule().radius;
            
            colliders[0].setGeometry(PxCapsuleGeometry(initialRadius, halfHeight));
        }
    }

    void PhysicsRigidbody::SetBoxColliderHalfExtents(const Vector3 extends)
    {
        if ((extends.x > 0. && extends.y > 0.0f && extends.z > 0.0f) && (extends.x < PX_MAX_F32 && extends.y < PX_MAX_F32 && extends.z < PX_MAX_F32))
        {
            const auto colliders = GetColliders();
            
            colliders[0].setGeometry(PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(extends)));
        }
    }
    
    BaseActor* PhysicsRigidbody::GetRigidbody() const
    {
        return m_Actor;
    }
    
    RigidbodyType PhysicsRigidbody::GetType() const
    {
        return static_cast<RigidbodyType>(m_Actor->getType());
    }
    
    bool PhysicsRigidbody::GetGravityFlag() const
    {
        return !m_Actor->getActorFlags().isSet(PxActorFlag::eDISABLE_GRAVITY);
    }
    
    bool PhysicsRigidbody::GetColliderVisualizationFlag() const
    {
        return m_Actor->getActorFlags().isSet(PxActorFlag::eVISUALIZATION);
    }
    
    float PhysicsRigidbody::GetStaticFriction() const
    {
        const auto colliders = GetColliders();
        const auto physicalMaterials = GetPhysicalMaterials(colliders);
        
        return physicalMaterials[0].getStaticFriction();
    }
    
    float PhysicsRigidbody::GetDynamicFriction() const
    {
        const auto colliders = GetColliders();
        const auto physicalMaterials = GetPhysicalMaterials(colliders);
        
        return physicalMaterials[0].getDynamicFriction();
    }
    
    float PhysicsRigidbody::GetRestitution() const
    {
        const auto colliders = GetColliders();
        const auto physicalMaterials = GetPhysicalMaterials(colliders);
        
        return physicalMaterials[0].getRestitution();
    }
    
    void PhysicsRigidbody::SetType(const RigidbodyType rigidbodyType)
    {
        if (static_cast<int>(m_Actor->getType()) != static_cast<int>(rigidbodyType))
        {
            const PxTransform initialPose = m_Actor->getGlobalPose();
            
            CleanupRigidbody();
        
            switch (rigidbodyType)
            {
            case RigidbodyType::Static:
                m_Actor = PhysicsBase::GetInstance().GetFactory()->createRigidStatic(initialPose);
                PhysicsBase::GetInstance().GetCollection()->addActor(*(reinterpret_cast<StaticActor*>(m_Actor)));
                SetGravityFlag(false);
                break;
            case RigidbodyType::Dynamic:
                m_Actor = PhysicsBase::GetInstance().GetFactory()->createRigidDynamic(initialPose);
                PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<DynamicActor*>(m_Actor)), 10);
                PhysicsBase::GetInstance().GetCollection()->addActor(*(reinterpret_cast<DynamicActor*>(m_Actor)));
                SetGravityFlag(true);
                break;
            default:
                break;
            }
            
            AttachCollider(ColliderType::Box);
        }
    }
    
    void PhysicsRigidbody::SetGravityFlag(const bool flag)
    {
        if (GetGravityFlag() != flag)
        {
            m_Actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !flag);
        }
    }
    
    void PhysicsRigidbody::SetColliderVisualizationFlag(const bool flag)
    {
        if (GetColliderVisualizationFlag() != flag)
        {
            m_Actor->setActorFlag(PxActorFlag::eVISUALIZATION, flag);
        }
    }
    
    void PhysicsRigidbody::SetStaticFriction(const float staticFriction)
    {
        if ((staticFriction >= 0.0f) && (staticFriction <= PX_MAX_F32))
        {
            const auto colliders = GetColliders();
            const auto physicalMaterials = GetPhysicalMaterials(colliders);
            
            physicalMaterials[0].setStaticFriction(staticFriction);
        }
    }
    
    void PhysicsRigidbody::SetDynamicFriction(const float dynamicFriction)
    {
        if ((dynamicFriction >= 0.0f) && (dynamicFriction <= PX_MAX_F32))
        {
            const auto colliders = GetColliders();
            const auto physicalMaterials = GetPhysicalMaterials(colliders);
            
            physicalMaterials[0].setDynamicFriction(dynamicFriction);
        }
    }
    
    void PhysicsRigidbody::SetRestitution(const float restitution)
    {
        if ((restitution >= 0.0f) && (restitution <= 1.0f))
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
            out << YAML::Key << "Rigidbody Type" << YAML::Value << static_cast<int>(component.GetType());
            out << YAML::Key << "Gravity Flag" << YAML::Value << component.GetGravityFlag();
            out << YAML::Key << "Collider Type" << YAML::Value << static_cast<int>(component.GetColliderType());

            switch (component.GetColliderType())
            {
            case ColliderType::Sphere:
                out << YAML::Key << "Sphere Radius" << YAML::Value << component.GetSphereColliderRadius();
                break;
            case ColliderType::Capsule:
                out << YAML::Key << "Capsule Radius" << YAML::Value << component.GetCapsuleColliderRadius();
                out << YAML::Key << "Capsule Half Height" << YAML::Value << component.GetCapsuleColliderHalfHeight();
                break;
            case ColliderType::Box:
                out << YAML::Key << "Box Half Extends" << YAML::Value << component.GetBoxColliderHalfExtents();
                break;
            default:
                break;
            }

            out << YAML::Key << "Offset Position" << YAML::Value << component.GetColliderLocalPosition();
            out << YAML::Key << "Offset Rotation" << YAML::Value << component.GetColliderLocalRotation();
            
            out << YAML::Key << "Static Friction" << YAML::Value << component.GetStaticFriction();
            out << YAML::Key << "Dynamic Friction" << YAML::Value << component.GetDynamicFriction();
            out << YAML::Key << "Restitution" << YAML::Value << component.GetRestitution();
        }

        void DeserializeData(YAML::Node& node, PhysicsRigidbody& component) override
        {
            component.SetColliderVisualizationFlag(node["Visualization Flag"].as<bool>());
            component.SetType(static_cast<RigidbodyType>(node["Rigidbody Type"].as<int>()));
            component.SetGravityFlag(node["Gravity Flag"].as<bool>());

            component.AttachCollider(static_cast<ColliderType>(node["Collider Type"].as<int>()));
            switch (component.GetColliderType())
            {
            case ColliderType::Sphere:
                component.SetSphereColliderRadius(node["Sphere Radius"].as<float>());
                break;
            case ColliderType::Capsule:
                component.SetCapsuleColliderRadius(node["Capsule Radius"].as<float>());
                component.SetCapsuleColliderHalfHeight(node["Capsule Half Height"].as<float>());
                break;
            case ColliderType::Box:
                component.SetBoxColliderHalfExtents(node["Box Half Extends"].as<Vector3>());
                break;
            default:
                break;
            } 

            component.SetColliderLocalPosition(node["Offset Position"].as<Vector3>());
            component.SetColliderLocalRotation(node["Offset Rotation"].as<Vector3>());

            component.SetStaticFriction(node["Static Friction"].as<float>());
            component.SetDynamicFriction(node["Dynamic Friction"].as<float>());
            component.SetRestitution(node["Restitution"].as<float>());

            component.Deinitialize();
        }
    };
}
