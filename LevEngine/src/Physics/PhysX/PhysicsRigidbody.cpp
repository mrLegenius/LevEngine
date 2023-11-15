#include "levpch.h"
#include "PhysicsRigidbody.h"
#include "PhysicsBase.h"
#include "PhysicsUtils.h"

constexpr auto DEFAULT_ACTOR_DENSITY    = 10;
constexpr auto DEFAULT_STATIC_FRICTION  = 0.5f;
constexpr auto DEFAULT_DYNAMIC_FRICTION = 0.5f;
constexpr auto DEFAULT_RESTITUTION      = 0.6f;
constexpr auto DEFAULT_SHAPE_SIZE       = 0.5f;

namespace LevEngine
{
    void PhysicsRigidbody::OnComponentConstruct(entt::registry& registry, entt::entity entity)
    {
        auto& rigidbody = registry.get<PhysicsRigidbody>(entity);
        // TODO: INITIAL TRANSFORM INITIALIZATION [NOW HAVE TO USE SetRigidBodyInitialPose()]
        rigidbody.rbActor = PhysicsBase::GetInstance().GetPhysics()->createRigidDynamic(PxTransform(PxIdentity));
        PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<PxRigidDynamic*>(rigidbody.rbActor)), DEFAULT_ACTOR_DENSITY);
        PhysicsBase::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidDynamic*>(rigidbody.rbActor)));
        rigidbody.AttachCollider(ColliderType::Box);
    }
    void PhysicsRigidbody::SetRigidbodyInitialPose(const Transform& transform)
    {
        const PxTransform pxTransform = PhysicsUtils::FromTransformToPxTransform(transform);
        rbActor->setGlobalPose(pxTransform);
    }
    
    void PhysicsRigidbody::OnComponentDestroy(entt::registry& registry, entt::entity entity)
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

    
    // multiple shapes //

    void PhysicsRigidbody::AttachMultipleCollider(const ColliderType& colliderType)
    {
        const auto nbShapes = rbActor->getNbShapes();
        if (nbShapes > MAX_NUM_RIGIDBODY_SHAPES)
        {
            Log::Debug("shape limit");
            return;
        }
    }
    
    // multiple shapes //

    
    Vector3 PhysicsRigidbody::GetShapeLocalPosition() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        return PhysicsUtils::FromPxVec3ToVector3(shapes[0]->getLocalPose().p);
    }

    void PhysicsRigidbody::SetShapeLocalPosition(const Vector3 position)
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        const auto rotation = shapes[0]->getLocalPose().q;
        shapes[0]->setLocalPose(PxTransform(PhysicsUtils::FromVector3ToPxVec3(position), rotation));
    }

    Vector3 PhysicsRigidbody::GetShapeLocalRotation() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);

        const auto quaternion = PhysicsUtils::FromPxQuatToQuaternion(shapes[0]->getLocalPose().q);
        
        return Math::ToDegrees(quaternion.ToEuler());
    }

    void PhysicsRigidbody::SetShapeLocalRotation(const Vector3 rotation)
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        const auto position = shapes[0]->getLocalPose().p;
        
        const auto quaternion = Quaternion::CreateFromYawPitchRoll(Math::ToRadians(rotation));
        
        shapes[0]->setLocalPose(PxTransform(position, PhysicsUtils::FromQuaternionToPxQuat(quaternion)));
    }
    
    ColliderType PhysicsRigidbody::GetColliderType() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        if (shapes[0]->getGeometry().getType() == PxGeometryType::eSPHERE)
        {
            return ColliderType::Sphere;
        }
        if (shapes[0]->getGeometry().getType() == PxGeometryType::eCAPSULE)
        {
            return ColliderType::Capsule;
        }
        if (shapes[0]->getGeometry().getType() == PxGeometryType::eBOX)
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
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);

        const PxGeometryHolder geom(shapes[0]->getGeometry());
        const auto radius = geom.sphere().radius;
        
        return radius;
    }

    float PhysicsRigidbody::GetCapsuleColliderRadius() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);

        const PxGeometryHolder geom(shapes[0]->getGeometry());
        const auto radius = geom.capsule().radius;
        
        return radius;
    }
    
    float PhysicsRigidbody::GetCapsuleColliderHalfHeight() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);

        const PxGeometryHolder geom(shapes[0]->getGeometry());
        const auto halfHeight = geom.capsule().halfHeight;
        
        return halfHeight;
    }

    Vector3 PhysicsRigidbody::GetBoxHalfExtends() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        const PxGeometryHolder geom(shapes[0]->getGeometry());
        const auto halfExtends = geom.box().halfExtents;

        return PhysicsUtils::FromPxVec3ToVector3(halfExtends);
    }

    
    void PhysicsRigidbody::SetSphereColliderRadius(const float radius)
    {
        if ((radius > 0.0f) && (radius <= PX_MAX_F32))
        {
            PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
            const auto nbShapes = rbActor->getNbShapes();
            rbActor->getShapes(shapes, nbShapes);
            
            shapes[0]->setGeometry(PxSphereGeometry(radius));
        }
    }
    
    void PhysicsRigidbody::SetCapsuleColliderRadius(const float radius)
    {
        if ((radius > 0.0f) && (radius <= PX_MAX_F32))
        {
            PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
            const auto nbShapes = rbActor->getNbShapes();
            rbActor->getShapes(shapes, nbShapes);
        
            const PxGeometryHolder geom(shapes[0]->getGeometry());
            const auto initialHeight = geom.capsule().halfHeight;
            
            shapes[0]->setGeometry(PxCapsuleGeometry(radius, initialHeight));
        }
    }
    
    void PhysicsRigidbody::SetCapsuleColliderHalfHeight(const float halfHeight)
    {
        if ((halfHeight >= 0.0f) && (halfHeight <= PX_MAX_F32))
        {
            PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
            const auto nbShapes = rbActor->getNbShapes();
            rbActor->getShapes(shapes, nbShapes);
        
            const PxGeometryHolder geom(shapes[0]->getGeometry());
            const auto initialRadius = geom.capsule().radius;
            
            shapes[0]->setGeometry(PxCapsuleGeometry(initialRadius, halfHeight));
        }
    }

    void PhysicsRigidbody::SetBoxHalfExtends(const Vector3 extends)
    {
        if ((extends.x > 0. && extends.y > 0.0f && extends.z > 0.0f)
            && (extends.x < PX_MAX_F32 && extends.y < PX_MAX_F32 && extends.z < PX_MAX_F32))
        {
            PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
            const auto nbShapes = rbActor->getNbShapes();
            rbActor->getShapes(shapes, nbShapes);
            
            shapes[0]->setGeometry(PxBoxGeometry(PhysicsUtils::FromVector3ToPxVec3(extends)));
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
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        PxMaterial* materials[MAX_NUM_RIGIDBODY_MATERIALS];
        const auto nbMaterials = shapes[0]->getNbMaterials();
        shapes[0]->getMaterials(materials, nbMaterials);
        
        return materials[0]->getStaticFriction();
    }
    
    float PhysicsRigidbody::GetDynamicFriction() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        PxMaterial* materials[MAX_NUM_RIGIDBODY_MATERIALS];
        const auto nbMaterials = shapes[0]->getNbMaterials();
        shapes[0]->getMaterials(materials, nbMaterials);
        
        return materials[0]->getDynamicFriction();
    }
    
    float PhysicsRigidbody::GetRestitution() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        PxMaterial* materials[MAX_NUM_RIGIDBODY_MATERIALS];
        const auto nbMaterials = shapes[0]->getNbMaterials();
        shapes[0]->getMaterials(materials, nbMaterials);
        
        return materials[0]->getRestitution();
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
            PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
            const auto nbShapes = rbActor->getNbShapes();
            rbActor->getShapes(shapes, nbShapes);
            
            PxMaterial* materials[MAX_NUM_RIGIDBODY_MATERIALS];
            const auto nbMaterials = shapes[0]->getNbMaterials();
            shapes[0]->getMaterials(materials, nbMaterials);
            
            materials[0]->setStaticFriction(staticFriction);
        }
    }
    
    void PhysicsRigidbody::SetDynamicFriction(const float dynamicFriction)
    {
        if ((dynamicFriction >= 0.0f) && (dynamicFriction <= PX_MAX_F32))
        {
            PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
            const auto nbShapes = rbActor->getNbShapes();
            rbActor->getShapes(shapes, nbShapes);
            
            PxMaterial* materials[MAX_NUM_RIGIDBODY_MATERIALS];
            const auto nbMaterials = shapes[0]->getNbMaterials();
            shapes[0]->getMaterials(materials, nbMaterials);
            
            materials[0]->setDynamicFriction(dynamicFriction);
        }
    }
    
    void PhysicsRigidbody::SetRestitution(const float restitution)
    {
        if ((restitution >= 0.0f) && (restitution <= 1.0f))
        {
            PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
            const auto nbShapes = rbActor->getNbShapes();
            rbActor->getShapes(shapes, nbShapes);
            
            PxMaterial* materials[MAX_NUM_RIGIDBODY_MATERIALS];
            const auto nbMaterials = shapes[0]->getNbMaterials();
            shapes[0]->getMaterials(materials, nbMaterials);
            
            materials[0]->setRestitution(restitution);
        }
    }
}
