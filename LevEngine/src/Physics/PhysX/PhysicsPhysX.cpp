#include "levpch.h"
#include "PhysicsPhysX.h"
#include "Renderer/DebugRender/DebugRender.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Physics/PhysX/PhysicsUtils.h"

constexpr auto DEFAULT_PVD_HOST = "127.0.0.1";
constexpr auto DEFAULT_PVD_PORT = 5425;
constexpr auto DEFAULT_PVD_CONNECT_TIMEOUT = 10;

constexpr auto DEFAULT_NUMBER_CPU_THREADS = 2;

constexpr auto MAX_NUM_RIGIDBODIES         = 20;
//constexpr auto MAX_NUM_RIGIDBODY_SCENES    = 5;
constexpr auto MAX_NUM_RIGIDBODY_SHAPES    = 5;
constexpr auto MAX_NUM_RIGIDBODY_MATERIALS = 5;

constexpr Vector3 DEFAULT_GRAVITY_SCALE    = {0.0f, -9.81f, 0.0f};
constexpr auto    DEFAULT_ACTOR_DENSITY    = 10;
constexpr auto    DEFAULT_STATIC_FRICTION  = 0.5f;
constexpr auto    DEFAULT_DYNAMIC_FRICTION = 0.5f;
constexpr auto    DEFAULT_RESTITUTION      = 0.6f;
constexpr auto    DEFAULT_SHAPE_SIZE       = 0.5f;


namespace LevEngine
{
    PhysicsPhysX PhysicsPhysX::physx;
    
    void PhysicsPhysX::InitPhysics()
    {
        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

        if (usePVD)
        {
            gPvd = PxCreatePvd(*gFoundation);
            PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(DEFAULT_PVD_HOST, DEFAULT_PVD_PORT, DEFAULT_PVD_CONNECT_TIMEOUT);
            gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);
        }
        
        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, gToleranceScale, true, gPvd);
        
        PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity = PhysicsUtils::FromVector3ToPxVec3(DEFAULT_GRAVITY_SCALE);
        gDispatcher = PxDefaultCpuDispatcherCreate(DEFAULT_NUMBER_CPU_THREADS);
        sceneDesc.cpuDispatcher	= gDispatcher;
        sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
        gScene = gPhysics->createScene(sceneDesc);

        if (useDebugRender)
        {
            gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
            gScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
            gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
        }
        
        if (usePVD)
        {
            PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
            if(pvdClient)
            {
                pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
                pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
                pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
            }
        }
    }
    PhysicsPhysX::PhysicsPhysX()
    {
        InitPhysics();
    }
    
    bool PhysicsPhysX::Advance(float deltaTime)
    {
        mAccumulator += deltaTime;
        if (mAccumulator < mStepSize)
        {
            return false;
        }
        mAccumulator -= mStepSize;
        physx.gScene->simulate(mStepSize);
        return true;
    }
    void PhysicsPhysX::StepPhysics(float deltaTime)
    {
        if (Advance(deltaTime))
        {
            physx.gScene->fetchResults(true);
        }
    }
    void PhysicsPhysX::UpdateTransforms(entt::registry& registry)
    {
        const auto view = registry.view<RigidbodyPhysX, Transform>();
        for (const auto entity : view)
        {
            auto [transform, rigidbody] = view.get<Transform, RigidbodyPhysX>(entity);
            const PxTransform actorPose = rigidbody.GetRigidbody()->getGlobalPose();
            //transform.SetWorldRotation(Quaternion(actorPose.q.x, actorPose.q.y, actorPose.q.z, actorPose.q.w));
            //transform.SetWorldPosition(Vector3(actorPose.p.x, actorPose.p.y, actorPose.p.z));
            transform.SetWorldRotation(PhysicsUtils::FromPxQuatToQuaternion(actorPose.q));
            transform.SetWorldPosition(PhysicsUtils::FromPxVec3ToVector3(actorPose.p));
        }
    }
    void PhysicsPhysX::DrawDebugLines()
    {
        const PxRenderBuffer& rb = physx.gScene->getRenderBuffer();
        auto a = rb.getNbLines();
        for (auto i = 0; i < rb.getNbLines(); i++)
        {
            const PxDebugLine& line = rb.getLines()[i];
            DebugRender::DrawLine(Vector3(line.pos0.x, line.pos0.y, line.pos0.z),
                                    Vector3(line.pos1.x, line.pos1.y, line.pos1.z),
                                        Color(1.0f, 0.0f, 0.0f, 1.0f));
        }
        for (auto i = 0; i < rb.getNbPoints(); i++)
        {
            const PxDebugPoint& point = rb.getPoints()[i];
            DebugRender::DrawPoint(Vector3(point.pos.x, point.pos.y, point.pos.z),
                                            Color(0.8f, 0.8f, 0.8f, 1.0f));
        }
        auto b = rb.getNbTriangles();
        for (auto i = 0; i < rb.getNbTriangles(); i++)
        {
            const PxDebugTriangle& tri = rb.getTriangles()[i];
            DebugRender::DrawLine(Vector3(tri.pos0.x, tri.pos0.y, tri.pos0.z),
                                    Vector3(tri.pos1.x, tri.pos1.y, tri.pos1.z),
                                        Color(1.0f, 0.0f, 0.0f, 1.0f));
            DebugRender::DrawLine(Vector3(tri.pos1.x, tri.pos1.y, tri.pos1.z),
                                    Vector3(tri.pos2.x, tri.pos2.y, tri.pos2.z),
                                        Color(1.0f, 0.0f, 0.0f, 1.0f));
            DebugRender::DrawLine(Vector3(tri.pos0.x, tri.pos0.y, tri.pos0.z),
                                    Vector3(tri.pos2.x, tri.pos2.y, tri.pos2.z),
                                        Color(1.0f, 0.0f, 0.0f, 1.0f));
        }
    }
    void PhysicsPhysX::Process(entt::registry& m_Registry, float deltaTime)
    {
        StepPhysics(deltaTime);
        UpdateTransforms(m_Registry);
        if (useDebugRender)
        {
            DrawDebugLines();
        }
    }
    
    void PhysicsPhysX::CleanupPhysics()
    {
        PX_RELEASE(gScene)
        PX_RELEASE(gDispatcher)
        PX_RELEASE(gPhysics)
        if(gPvd)
        {
            PxPvdTransport* transport = gPvd->getTransport();
            PX_RELEASE(gPvd)
            PX_RELEASE(transport)
        }
        PX_RELEASE(gFoundation)
    }
    PhysicsPhysX::~PhysicsPhysX()
    {
        CleanupPhysics();
    }

    PhysicsPhysX& PhysicsPhysX::GetInstance()
    {
        return physx;
    }
    
    PxPhysics* PhysicsPhysX::GetPhysics() const
    {
        return gPhysics;
    }

    PxScene* PhysicsPhysX::GetScene() const
    {
        return gScene;
    }

    Vector3 PhysicsPhysX::GetGravity() const
    {
        const auto gravity = gScene->getGravity();
        //return Vector3(gravity.x, gravity.y, gravity.z);
        return PhysicsUtils::FromPxVec3ToVector3(gravity);
    }

    void PhysicsPhysX::SetGravity(const Vector3 newGravity)
    {
        const auto gravity = PhysicsUtils::FromVector3ToPxVec3(newGravity);
        //gScene->setGravity(PxVec3(gravity.x, gravity.y, gravity.z));
        gScene->setGravity(gravity);
    }

    
    
    void RigidbodyPhysX::OnComponentConstruct(entt::registry& registry, entt::entity entity)
    {
        auto& rigidbody = registry.get<RigidbodyPhysX>(entity);
        const auto& transform = registry.get<Transform>(entity);
        /*
        const PxTransform pxTransform
        {
            PxVec3(transform.GetWorldPosition().x, transform.GetWorldPosition().y, transform.GetWorldPosition().z),
            PxQuat(transform.GetWorldRotation().x, transform.GetWorldRotation().y, transform.GetWorldRotation().z, transform.GetWorldRotation().w)
        };
        */
        const PxTransform pxTransform = PhysicsUtils::FromTransformToPxTransform(transform);
        rigidbody.rbActor = PhysicsPhysX::GetInstance().GetPhysics()->createRigidDynamic(pxTransform);
        PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<PxRigidDynamic*>(rigidbody.rbActor)), DEFAULT_ACTOR_DENSITY);
        PhysicsPhysX::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidDynamic*>(rigidbody.rbActor)));
        rigidbody.AttachCollider(ColliderType::Box);
    }
    void RigidbodyPhysX::OnComponentDestroy(entt::registry& registry, entt::entity entity)
    {
        auto& rigidbody = registry.get<RigidbodyPhysX>(entity);
        rigidbody.CleanupRigidbody();
    } 

    void RigidbodyPhysX::SetRigidbodyInitialPose(const Transform& transform)
    {
        /*
        const PxTransform pxTransform
        {
            PxVec3(transform.GetWorldPosition().x, transform.GetWorldPosition().y, transform.GetWorldPosition().z),
            PxQuat(transform.GetWorldRotation().x, transform.GetWorldRotation().y, transform.GetWorldRotation().z, transform.GetWorldRotation().w)
        };
        */
        const PxTransform pxTransform = PhysicsUtils::FromTransformToPxTransform(transform);
        rbActor->setGlobalPose(pxTransform);
    }
    
    void RigidbodyPhysX::CleanupRigidbody()
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

    ColliderType RigidbodyPhysX::GetColliderType() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        return static_cast<ColliderType>(static_cast<int>(shapes[0]->getGeometry().getType()));
    }

    void RigidbodyPhysX::AttachCollider(const ColliderType& colliderType)
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
        
        const auto material = PhysicsPhysX::GetInstance().GetPhysics()->createMaterial(DEFAULT_STATIC_FRICTION, DEFAULT_DYNAMIC_FRICTION, DEFAULT_RESTITUTION);
        PxShape* shape = nullptr;
        switch (colliderType)
        {
        case ColliderType::Sphere:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxSphereGeometry(DEFAULT_SHAPE_SIZE), *material, true);
            break;
        case ColliderType::Plane:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxPlaneGeometry(), *material, true);
            break;
        case ColliderType::Capsule:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxCapsuleGeometry(DEFAULT_SHAPE_SIZE, DEFAULT_SHAPE_SIZE), *material, true);
            break;
        case ColliderType::Box:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxBoxGeometry(DEFAULT_SHAPE_SIZE, DEFAULT_SHAPE_SIZE, DEFAULT_SHAPE_SIZE), *material, true);
            break;
        default:
            break;
        }
        rbActor->attachShape(*shape);
        shape->release();
        material->release();
    }
    
    void RigidbodyPhysX::DetachCollider()
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        for (auto i = 0; i < nbShapes; i++)
        {
            rbActor->detachShape(*shapes[i]);
        }
    }

    float RigidbodyPhysX::GetSphereColliderRadius() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);

        const PxGeometryHolder geom(shapes[0]->getGeometry());
        const auto radius = geom.sphere().radius;
        
        return radius;
    }

    float RigidbodyPhysX::GetCapsuleColliderRadius() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);

        const PxGeometryHolder geom(shapes[0]->getGeometry());
        const auto radius = geom.capsule().radius;
        
        return radius;
    }
    
    float RigidbodyPhysX::GetCapsuleColliderHalfHeight() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);

        const PxGeometryHolder geom(shapes[0]->getGeometry());
        const auto halfHeight = geom.capsule().halfHeight;
        
        return halfHeight;
    }

    float RigidbodyPhysX::GetBoxColliderHalfExtendX() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        const PxGeometryHolder geom(shapes[0]->getGeometry());
        const auto initialHalfExtendX = geom.box().halfExtents.x;
        
        return initialHalfExtendX;
    }

    float RigidbodyPhysX::GetBoxColliderHalfExtendY() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        const PxGeometryHolder geom(shapes[0]->getGeometry());
        const auto initialHalfExtendY = geom.box().halfExtents.y;
        
        return initialHalfExtendY;
    }

    float RigidbodyPhysX::GetBoxColliderHalfExtendZ() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        const PxGeometryHolder geom(shapes[0]->getGeometry());
        const auto initialHalfExtendZ = geom.box().halfExtents.z;
        
        return initialHalfExtendZ;
    }
    
    void RigidbodyPhysX::SetSphereColliderRadius(float radius)
    {
        if ((radius > 0.0f) && (radius <= PX_MAX_F32))
        {
            PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
            const auto nbShapes = rbActor->getNbShapes();
            rbActor->getShapes(shapes, nbShapes);
            
            shapes[0]->setGeometry(PxSphereGeometry(radius));
        }
    }
    
    void RigidbodyPhysX::SetCapsuleColliderRadius(float radius)
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
    
    void RigidbodyPhysX::SetCapsuleColliderHalfHeight(float halfHeight)
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
    
    void RigidbodyPhysX::SetBoxColliderHalfExtendX(float halfExtendX)
    {
        if ((halfExtendX > 0.0f) && (halfExtendX <= PX_MAX_F32))
        {
            PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
            const auto nbShapes = rbActor->getNbShapes();
            rbActor->getShapes(shapes, nbShapes);
        
            const PxGeometryHolder geom(shapes[0]->getGeometry());
            const auto initialHalfExtendY = geom.box().halfExtents.y;
            const auto initialHalfExtendZ = geom.box().halfExtents.z;
            
            shapes[0]->setGeometry(PxBoxGeometry(halfExtendX, initialHalfExtendY, initialHalfExtendZ));
        }
    }

    void RigidbodyPhysX::SetBoxColliderHalfExtendY(float halfExtendY)
    {
        if ((halfExtendY > 0.0f) && (halfExtendY <= PX_MAX_F32))
        {
            PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
            const auto nbShapes = rbActor->getNbShapes();
            rbActor->getShapes(shapes, nbShapes);
        
            const PxGeometryHolder geom(shapes[0]->getGeometry());
            const auto initialHalfExtendX = geom.box().halfExtents.x;
            const auto initialHalfExtendZ = geom.box().halfExtents.z;
            
            shapes[0]->setGeometry(PxBoxGeometry(initialHalfExtendX, halfExtendY, initialHalfExtendZ));
        }
    }

    void RigidbodyPhysX::SetBoxColliderHalfExtendZ(float halfExtendZ)
    {
        if ((halfExtendZ > 0.0f) && (halfExtendZ <= PX_MAX_F32))
        {
            PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
            const auto nbShapes = rbActor->getNbShapes();
            rbActor->getShapes(shapes, nbShapes);
        
            const PxGeometryHolder geom(shapes[0]->getGeometry());
            const auto initialHalfExtendX = geom.box().halfExtents.x;
            const auto initialHalfExtendY = geom.box().halfExtents.y;
            
            shapes[0]->setGeometry(PxBoxGeometry(initialHalfExtendX, initialHalfExtendY, halfExtendZ));
        }
    }
    
    PxRigidActor* RigidbodyPhysX::GetRigidbody() const
    {
        return rbActor;
    }
    
    RigidbodyType RigidbodyPhysX::GetRigidbodyType() const
    {
        return static_cast<RigidbodyType>(rbActor->getType());
    }
    
    bool RigidbodyPhysX::GetRigidbodyGravityFlag() const
    {
        return !rbActor->getActorFlags().isSet(PxActorFlag::eDISABLE_GRAVITY);
    }
    
    bool RigidbodyPhysX::GetColliderVisualizationFlag() const
    {
        return rbActor->getActorFlags().isSet(PxActorFlag::eVISUALIZATION);
    }
    
    float RigidbodyPhysX::GetStaticFriction() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        PxMaterial* materials[MAX_NUM_RIGIDBODY_MATERIALS];
        const auto nbMaterials = shapes[0]->getNbMaterials();
        shapes[0]->getMaterials(materials, nbMaterials);
        
        return materials[0]->getStaticFriction();
    }
    
    float RigidbodyPhysX::GetDynamicFriction() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        PxMaterial* materials[MAX_NUM_RIGIDBODY_MATERIALS];
        const auto nbMaterials = shapes[0]->getNbMaterials();
        shapes[0]->getMaterials(materials, nbMaterials);
        
        return materials[0]->getDynamicFriction();
    }
    
    float RigidbodyPhysX::GetRestitution() const
    {
        PxShape* shapes[MAX_NUM_RIGIDBODY_SHAPES];
        const auto nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        PxMaterial* materials[MAX_NUM_RIGIDBODY_MATERIALS];
        const auto nbMaterials = shapes[0]->getNbMaterials();
        shapes[0]->getMaterials(materials, nbMaterials);
        
        return materials[0]->getRestitution();
    }
    
    void RigidbodyPhysX::SetRigidbodyType(const RigidbodyType rigidbodyType)
    {
        if (static_cast<int>(rbActor->getType()) != static_cast<int>(rigidbodyType))
        {
            PxTransform initialPose = rbActor->getGlobalPose();
            CleanupRigidbody();
        
            switch (rigidbodyType)
            {
            case RigidbodyType::Static:
                rbActor = PhysicsPhysX::GetInstance().GetPhysics()->createRigidStatic(initialPose);
                PhysicsPhysX::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidStatic*>(rbActor)));
                SetRigidbodyGravityFlag(false);
                break;
            case RigidbodyType::Dynamic:
                rbActor = PhysicsPhysX::GetInstance().GetPhysics()->createRigidDynamic(initialPose);
                PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<PxRigidDynamic*>(rbActor)), 10);
                PhysicsPhysX::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidDynamic*>(rbActor)));
                SetRigidbodyGravityFlag(true);
                break;
            default:
                break;
            }
            AttachCollider(ColliderType::Box);
        }
    }
    
    void RigidbodyPhysX::SetRigidbodyGravityFlag(const bool flag)
    {
        if (GetRigidbodyGravityFlag() != flag)
        {
            rbActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !flag);
        }
    }
    
    void RigidbodyPhysX::SetColliderVisualizationFlag(const bool flag)
    {
        if (GetColliderVisualizationFlag() != flag)
        {
            rbActor->setActorFlag(PxActorFlag::eVISUALIZATION, flag);
        }
    }
    
    void RigidbodyPhysX::SetStaticFriction(const float staticFriction)
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
    
    void RigidbodyPhysX::SetDynamicFriction(const float dynamicFriction)
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
    
    void RigidbodyPhysX::SetRestitution(const float restitution)
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

    
    
    class RigidbodyPhysXSerializer final : public ComponentSerializer<RigidbodyPhysX, RigidbodyPhysXSerializer>
    {
    protected:
        const char* GetKey() override
        {
            return "RigidbodyPhysX";
        }

        void SerializeData(YAML::Emitter& out, const RigidbodyPhysX& component) override
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
            case ColliderType::Plane:
                break;
            case ColliderType::Capsule:
                out << YAML::Key << "Capsule Radius" << YAML::Value << component.GetCapsuleColliderRadius();
                out << YAML::Key << "Capsule Half Height" << YAML::Value << component.GetCapsuleColliderHalfHeight();
                break;
            case ColliderType::Box:
                out << YAML::Key << "Box Extend X" << YAML::Value << component.GetBoxColliderHalfExtendX();
                out << YAML::Key << "Box Extend Y" << YAML::Value << component.GetBoxColliderHalfExtendY();
                out << YAML::Key << "Box Extend Z" << YAML::Value << component.GetBoxColliderHalfExtendZ();
                break;
            default:
                break;
            }

            out << YAML::Key << "Static Friction" << YAML::Value << component.GetStaticFriction();
            out << YAML::Key << "Dynamic Friction" << YAML::Value << component.GetDynamicFriction();
            out << YAML::Key << "Restitution" << YAML::Value << component.GetRestitution();
        }

        void DeserializeData(YAML::Node& node, RigidbodyPhysX& component) override
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
            case ColliderType::Plane:
                break;
            case ColliderType::Capsule:
                component.SetCapsuleColliderRadius(node["Capsule Radius"].as<float>());
                component.SetCapsuleColliderHalfHeight(node["Capsule Half Height"].as<float>());
                break;
            case ColliderType::Box:
                component.SetBoxColliderHalfExtendX(node["Box Extend X"].as<float>());
                component.SetBoxColliderHalfExtendY(node["Box Extend Y"].as<float>());
                component.SetBoxColliderHalfExtendZ(node["Box Extend Z"].as<float>());
                break;
            default:
                break;
            }

            component.SetStaticFriction(node["Static Friction"].as<float>());
            component.SetDynamicFriction(node["Dynamic Friction"].as<float>());
            component.SetRestitution(node["Restitution"].as<float>());
        }
    };
}

