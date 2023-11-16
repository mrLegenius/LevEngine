#include "levpch.h"
#include "PhysicsRigidbody.h"
#include "PhysicsBase.h"
#include "PhysicsUtils.h"
#include "Scene/Components/ComponentSerializer.h"

constexpr auto DEFAULT_ACTOR_DENSITY    = 10;
constexpr auto DEFAULT_STATIC_FRICTION  = 0.5f;
constexpr auto DEFAULT_DYNAMIC_FRICTION = 0.5f;
constexpr auto DEFAULT_RESTITUTION      = 0.6f;
constexpr auto DEFAULT_SHAPE_SIZE       = 0.5f;

namespace LevEngine
{
    PhysicsRigidbody::PhysicsRigidbody()
    {
        rbActor = PhysicsBase::GetInstance().GetPhysics()->createRigidDynamic(PxTransform(PxIdentity));
        PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<PxRigidDynamic*>(rbActor)), DEFAULT_ACTOR_DENSITY);
        PhysicsBase::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidDynamic*>(rbActor)));
        AttachCollider(ColliderType::Box);
    }

    void PhysicsRigidbody::SetRigidbodyInitialPose(const Transform& transform)
    {
        const PxTransform pxTransform = PhysicsUtils::FromTransformToPxTransform(transform);
        rbActor->setGlobalPose(pxTransform);
    }
    
    void PhysicsRigidbody::Init(Transform& transform)
    {
        if (IsInitialized())
        {
            return;
        }
        
        SetRigidbodyInitialPose(transform);
        
        m_IsInited = true;
    }

    bool PhysicsRigidbody::IsInitialized() const
    {
        return m_IsInited;
    }

    void PhysicsRigidbody::ResetInit()
    {
        m_IsInited = false;
    }
    
    void PhysicsRigidbody::OnDestroy(entt::registry& registry, entt::entity entity)
    {
        auto& rigidbody = registry.get<PhysicsRigidbody>(entity);
        rigidbody.CleanupRigidbody();
    }
    
    void PhysicsRigidbody::CleanupRigidbody()
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        for(auto i = 0; i < nbShapes; ++i)
        {
            rbActor->detachShape(*shapes[i]);
        }
        
        rbActor->getScene()->removeActor(*rbActor);
        
        PX_RELEASE(rbActor);
    }

    PxShape* PhysicsRigidbody::GetActorShapes() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);

        return *shapes;
    }

    PxMaterial* PhysicsRigidbody::GetShapeMaterials(const PxShape* shapes) const
    {
        PxMaterial* materials[MAX_NUM_RIGIDBODY_MATERIALS];
        const auto nbMaterials = shapes[0].getNbMaterials();
        shapes[0].getMaterials(materials, nbMaterials);

        return *materials;
    }

    
    Vector3 PhysicsRigidbody::GetShapeLocalPosition() const
    {
        const auto shapes = GetActorShapes();
        
        return PhysicsUtils::FromPxVec3ToVector3(shapes[0].getLocalPose().p);
    }

    void PhysicsRigidbody::SetShapeLocalPosition(const Vector3 position)
    {
        const auto shapes = GetActorShapes();
        const auto rotation = shapes[0].getLocalPose().q;
        
        shapes[0].setLocalPose(PxTransform(PhysicsUtils::FromVector3ToPxVec3(position), rotation));
    }

    Vector3 PhysicsRigidbody::GetShapeLocalRotation() const
    {
        const auto shapes = GetActorShapes();
        const auto quaternion = PhysicsUtils::FromPxQuatToQuaternion(shapes[0].getLocalPose().q);
        
        return Math::ToDegrees(quaternion.ToEuler());
    }

    void PhysicsRigidbody::SetShapeLocalRotation(const Vector3 rotation)
    {
        const auto shapes = GetActorShapes();
        const auto position = shapes[0].getLocalPose().p;
        const auto quaternion = Quaternion::CreateFromYawPitchRoll(Math::ToRadians(rotation));
        
        shapes[0].setLocalPose(PxTransform(position, PhysicsUtils::FromQuaternionToPxQuat(quaternion)));
    }
    
    ColliderType PhysicsRigidbody::GetColliderType() const
    {
        const auto shapes = GetActorShapes();
        
        if (shapes[0].getGeometry().getType() == PxGeometryType::eSPHERE)
        {
            return ColliderType::Sphere;
        }
        if (shapes[0].getGeometry().getType() == PxGeometryType::eCAPSULE)
        {
            return ColliderType::Capsule;
        }
        if (shapes[0].getGeometry().getType() == PxGeometryType::eBOX)
        {
            return ColliderType::Box;
        }
    }

    void PhysicsRigidbody::AttachCollider(const ColliderType& colliderType)
    {
        if (rbActor->getNbShapes() > 0)
        {
            if (rbActor->getNbShapes() > 0)
            {
                if (GetColliderType() == colliderType)
                {
                    return;
                }
                if (rbActor->getNbShapes() >= MAX_NUM_RIGIDBODY_SHAPES)
                {
                    return;
                }
                DetachCollider();
            }
        }
        
        const auto material = PhysicsBase::GetInstance().GetPhysics()->createMaterial(DEFAULT_STATIC_FRICTION, DEFAULT_DYNAMIC_FRICTION, DEFAULT_RESTITUTION);
        PxShape* shape = nullptr;
        switch (colliderType)
        {
        case ColliderType::Sphere:
            shape = PhysicsBase::GetInstance().GetPhysics()->createShape(PxSphereGeometry(DEFAULT_SHAPE_SIZE), *material, true);
            break;
        case ColliderType::Capsule:
            shape = PhysicsBase::GetInstance().GetPhysics()->createShape(PxCapsuleGeometry(DEFAULT_SHAPE_SIZE, DEFAULT_SHAPE_SIZE), *material, true);
            break;
        case ColliderType::Box:
            shape = PhysicsBase::GetInstance().GetPhysics()->createShape(PxBoxGeometry(DEFAULT_SHAPE_SIZE, DEFAULT_SHAPE_SIZE, DEFAULT_SHAPE_SIZE), *material, true);
            break;
        default:
            break;
        }
        
        rbActor->attachShape(*shape);
        shape->release();
        material->release();
    }
    
    void PhysicsRigidbody::DetachCollider()
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        for (auto i = 0; i < nbShapes; i++)
        {
            rbActor->detachShape(*shapes[i]);
        }
    }

    float PhysicsRigidbody::GetSphereColliderRadius() const
    {
        const auto shapes = GetActorShapes();
        const PxGeometryHolder geom(shapes[0].getGeometry());
        const auto radius = geom.sphere().radius;
        
        return radius;
    }

    float PhysicsRigidbody::GetCapsuleColliderRadius() const
    {
        const auto shapes = GetActorShapes();
        const PxGeometryHolder geom(shapes[0].getGeometry());
        const auto radius = geom.capsule().radius;
        
        return radius;
    }
    
    float PhysicsRigidbody::GetCapsuleColliderHalfHeight() const
    {
        const auto shapes = GetActorShapes();
        const PxGeometryHolder geom(shapes[0].getGeometry());
        const auto halfHeight = geom.capsule().halfHeight;
        
        return halfHeight;
    }

    Vector3 PhysicsRigidbody::GetBoxHalfExtends() const
    {
        const auto shapes = GetActorShapes();
        const PxGeometryHolder geom(shapes[0].getGeometry());
        const auto halfExtends = geom.box().halfExtents;

        return PhysicsUtils::FromPxVec3ToVector3(halfExtends);
    }

    
    void PhysicsRigidbody::SetSphereColliderRadius(const float radius)
    {
        if ((radius > 0.0f) && (radius <= PX_MAX_F32))
        {
            const auto shapes = GetActorShapes();
            
            shapes[0].setGeometry(PxSphereGeometry(radius));
        }
    }
    
    void PhysicsRigidbody::SetCapsuleColliderRadius(const float radius)
    {
        if ((radius > 0.0f) && (radius <= PX_MAX_F32))
        {
            const auto shapes = GetActorShapes();
            const PxGeometryHolder geom(shapes[0].getGeometry());
            const auto initialHeight = geom.capsule().halfHeight;
            
            shapes[0].setGeometry(PxCapsuleGeometry(radius, initialHeight));
        }
    }
    
    void PhysicsRigidbody::SetCapsuleColliderHalfHeight(const float halfHeight)
    {
        if ((halfHeight >= 0.0f) && (halfHeight <= PX_MAX_F32))
        {
            const auto shapes = GetActorShapes();
            const PxGeometryHolder geom(shapes[0].getGeometry());
            const auto initialRadius = geom.capsule().radius;
            
            shapes[0].setGeometry(PxCapsuleGeometry(initialRadius, halfHeight));
        }
    }

    void PhysicsRigidbody::SetBoxHalfExtends(const Vector3 extends)
    {
        if ((extends.x > 0. && extends.y > 0.0f && extends.z > 0.0f) && (extends.x < PX_MAX_F32 && extends.y < PX_MAX_F32 && extends.z < PX_MAX_F32))
        {
            const auto shapes = GetActorShapes();
            
            shapes[0].setGeometry(PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(extends)));
        }
    }

    
    PxRigidActor* PhysicsRigidbody::GetRigidbody() const
    {
        return rbActor;
    }
    
    RigidbodyType PhysicsRigidbody::GetRigidbodyType() const
    {
        return static_cast<RigidbodyType>(rbActor->getType());
    }
    
    bool PhysicsRigidbody::GetRigidbodyGravityFlag() const
    {
        return !rbActor->getActorFlags().isSet(PxActorFlag::eDISABLE_GRAVITY);
    }
    
    bool PhysicsRigidbody::GetColliderVisualizationFlag() const
    {
        return rbActor->getActorFlags().isSet(PxActorFlag::eVISUALIZATION);
    }
    
    float PhysicsRigidbody::GetStaticFriction() const
    {
        const auto shapes = GetActorShapes();
        const auto materials = GetShapeMaterials(shapes);
        
        return materials[0].getStaticFriction();
    }
    
    float PhysicsRigidbody::GetDynamicFriction() const
    {
        const auto shapes = GetActorShapes();
        const auto materials = GetShapeMaterials(shapes);
        
        return materials[0].getDynamicFriction();
    }
    
    float PhysicsRigidbody::GetRestitution() const
    {
        const auto shapes = GetActorShapes();
        const auto materials = GetShapeMaterials(shapes);
        
        return materials[0].getRestitution();
    }
    
    void PhysicsRigidbody::SetRigidbodyType(const RigidbodyType rigidbodyType)
    {
        if (static_cast<int>(rbActor->getType()) != static_cast<int>(rigidbodyType))
        {
            PxTransform initialPose = rbActor->getGlobalPose();
            CleanupRigidbody();
        
            switch (rigidbodyType)
            {
            case RigidbodyType::Static:
                rbActor = PhysicsBase::GetInstance().GetPhysics()->createRigidStatic(initialPose);
                PhysicsBase::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidStatic*>(rbActor)));
                SetRigidbodyGravityFlag(false);
                break;
            case RigidbodyType::Dynamic:
                rbActor = PhysicsBase::GetInstance().GetPhysics()->createRigidDynamic(initialPose);
                PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<PxRigidDynamic*>(rbActor)), 10);
                PhysicsBase::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidDynamic*>(rbActor)));
                SetRigidbodyGravityFlag(true);
                break;
            default:
                break;
            }
            
            AttachCollider(ColliderType::Box);
        }
    }
    
    void PhysicsRigidbody::SetRigidbodyGravityFlag(const bool flag)
    {
        if (GetRigidbodyGravityFlag() != flag)
        {
            rbActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !flag);
        }
    }
    
    void PhysicsRigidbody::SetColliderVisualizationFlag(const bool flag)
    {
        if (GetColliderVisualizationFlag() != flag)
        {
            rbActor->setActorFlag(PxActorFlag::eVISUALIZATION, flag);
        }
    }
    
    void PhysicsRigidbody::SetStaticFriction(const float staticFriction)
    {
        if ((staticFriction >= 0.0f) && (staticFriction <= PX_MAX_F32))
        {
            const auto shapes = GetActorShapes();
            const auto materials = GetShapeMaterials(shapes);
            
            materials[0].setStaticFriction(staticFriction);
        }
    }
    
    void PhysicsRigidbody::SetDynamicFriction(const float dynamicFriction)
    {
        if ((dynamicFriction >= 0.0f) && (dynamicFriction <= PX_MAX_F32))
        {
            const auto shapes = GetActorShapes();
            const auto materials = GetShapeMaterials(shapes);
            
            materials[0].setDynamicFriction(dynamicFriction);
        }
    }
    
    void PhysicsRigidbody::SetRestitution(const float restitution)
    {
        if ((restitution >= 0.0f) && (restitution <= 1.0f))
        {
            const auto shapes = GetActorShapes();
            const auto materials = GetShapeMaterials(shapes);
            
            materials[0].setRestitution(restitution);
        }
    }
    
    class PhysicsRigidbodySerializer final : public ComponentSerializer<PhysicsRigidbody, PhysicsRigidbodySerializer>
    {
    protected:
        const char* GetKey() override
        {
            return "Physics Rigidbody";
        }

        void SerializeData(YAML::Emitter& out, const PhysicsRigidbody& component) override
        {
            out << YAML::Key << "Visualization Flag" << YAML::Value << component.GetColliderVisualizationFlag();
            out << YAML::Key << "Rigidbody Type" << YAML::Value << static_cast<int>(component.GetRigidbodyType());
            out << YAML::Key << "Gravity Flag" << YAML::Value << component.GetRigidbodyGravityFlag();
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
                out << YAML::Key << "Box Half Extends" << YAML::Value << component.GetBoxHalfExtends();
                break;
            default:
                break;
            }

            out << YAML::Key << "Offset Position" << YAML::Value << component.GetShapeLocalPosition();
            out << YAML::Key << "Offset Rotation" << YAML::Value << component.GetShapeLocalRotation();
            
            out << YAML::Key << "Static Friction" << YAML::Value << component.GetStaticFriction();
            out << YAML::Key << "Dynamic Friction" << YAML::Value << component.GetDynamicFriction();
            out << YAML::Key << "Restitution" << YAML::Value << component.GetRestitution();
        }

        void DeserializeData(YAML::Node& node, PhysicsRigidbody& component) override
        {
            component.SetColliderVisualizationFlag(node["Visualization Flag"].as<bool>());
            component.SetRigidbodyType(static_cast<RigidbodyType>(node["Rigidbody Type"].as<int>()));
            component.SetRigidbodyGravityFlag(node["Gravity Flag"].as<bool>());

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
                component.SetBoxHalfExtends(node["Box Half Extends"].as<Vector3>());
                break;
            default:
                break;
            } 

            component.SetShapeLocalPosition(node["Offset Position"].as<Vector3>());
            component.SetShapeLocalRotation(node["Offset Rotation"].as<Vector3>());

            component.SetStaticFriction(node["Static Friction"].as<float>());
            component.SetDynamicFriction(node["Dynamic Friction"].as<float>());
            component.SetRestitution(node["Restitution"].as<float>());

            component.ResetInit();
        }
    };
}
