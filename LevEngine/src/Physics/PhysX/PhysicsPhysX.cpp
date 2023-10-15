#include "levpch.h"
#include "PhysicsPhysX.h"
#include "Renderer/DebugRender/DebugRender.h"
#include "Scene/Components/ComponentSerializer.h"

#define MAX_NUM_ACTOR_SHAPES    128
#define MAX_NUM_ACTOR_MATERIALS 128

namespace LevEngine
{
    // --- PhysicsPhysX --- //
    PhysicsPhysX PhysicsPhysX::physx;
    
    void PhysicsPhysX::InitPhysics()
    {
        psFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, psAllocator, psErrorCallback);

        if (bDebug)
        {
            psPvd = PxCreatePvd(*psFoundation);
            PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
            psPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);
        }
        
        psPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *psFoundation, psToleranceScale, true, psPvd);
        
        PxSceneDesc sceneDesc(psPhysics->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
        psDispatcher = PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher	= psDispatcher;
        sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
        psScene = psPhysics->createScene(sceneDesc);

        if (bDebug)
        {
            psScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
            psScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
            psScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

            PxPvdSceneClient* pvdClient = psScene->getScenePvdClient();
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
        if (mAccumulator < mStepSize) { return false; }
        mAccumulator -= mStepSize;
        physx.psScene->simulate(mStepSize);
        return true;
    }
    void PhysicsPhysX::StepPhysics(float deltaTime)
    {
        if (Advance(deltaTime)) { physx.psScene->fetchResults(true); }
    }
    void PhysicsPhysX::UpdateTransforms(entt::registry& registry)
    {
        const auto view = registry.view<RigidbodyPhysX, Transform>();
        for (const auto entity : view)
        {
            auto [transform, rigidbody] = view.get<Transform, RigidbodyPhysX>(entity);
            PxShape* shapes[128];
            const PxU32 nbShapes = rigidbody.GetActor()->getNbShapes();
            rigidbody.GetActor()->getShapes(shapes, nbShapes);
            for(PxU32 j = 0; j < nbShapes; ++j)
            {
                const PxTransform shapePose(PxShapeExt::getGlobalPose(*shapes[j], *rigidbody.GetActor()));
                transform.SetWorldRotation(Quaternion(shapePose.q.x, shapePose.q.y, shapePose.q.z, shapePose.q.w));
                transform.SetWorldPosition(Vector3(shapePose.p.x, shapePose.p.y,shapePose.p.z));
            }
        }
    }
    void PhysicsPhysX::DrawDebugLines()
    {
        const PxRenderBuffer& rb = physx.psScene->getRenderBuffer();
        auto a = rb.getNbLines();
        for (PxU32 i = 0; i < rb.getNbLines(); i++)
        {
            const PxDebugLine& line = rb.getLines()[i];
            DebugRender::DrawLine(DirectX::SimpleMath::Vector3(line.pos0.x, line.pos0.y, line.pos0.z),
                                  DirectX::SimpleMath::Vector3(line.pos1.x, line.pos1.y, line.pos1.z),
                                  Color(0.8f, 0.8f, 0.8f, 1.0f));
        }
        for (PxU32 i = 0; i < rb.getNbPoints(); i++)
        {
            const PxDebugPoint& point = rb.getPoints()[i];
            DebugRender::DrawPoint(DirectX::SimpleMath::Vector3(point.pos.x, point.pos.y, point.pos.z),
                Color(0.8f, 0.8f, 0.8f, 1.0f));
        }
        auto b = rb.getNbTriangles();
        for (PxU32 i = 0; i < rb.getNbTriangles(); i++)
        {
            const PxDebugTriangle& tri = rb.getTriangles()[i];
            DebugRender::DrawLine(DirectX::SimpleMath::Vector3(tri.pos0.x, tri.pos0.y, tri.pos0.z),
                                  DirectX::SimpleMath::Vector3(tri.pos1.x, tri.pos1.y, tri.pos1.z),
                                  Color(1.0f, 0.0f, 0.0f, 1.0f));
            DebugRender::DrawLine(DirectX::SimpleMath::Vector3(tri.pos1.x, tri.pos1.y, tri.pos1.z),
                                  DirectX::SimpleMath::Vector3(tri.pos2.x, tri.pos2.y, tri.pos2.z),
                                  Color(1.0f, 0.0f, 0.0f, 1.0f));
            DebugRender::DrawLine(DirectX::SimpleMath::Vector3(tri.pos0.x, tri.pos0.y, tri.pos0.z),
                                  DirectX::SimpleMath::Vector3(tri.pos2.x, tri.pos2.y, tri.pos2.z),
                                  Color(1.0f, 0.0f, 0.0f, 1.0f));
        }
    }
    void PhysicsPhysX::Process(entt::registry& m_Registry, float deltaTime)
    {
        StepPhysics(deltaTime);
        UpdateTransforms(m_Registry);
        DrawDebugLines();
    }
    
    void PhysicsPhysX::CleanupPhysics()
    {
        PX_RELEASE(psScene)
        PX_RELEASE(psDispatcher)
        PX_RELEASE(psPhysics)
        if(psPvd)
        {
            PxPvdTransport* transport = psPvd->getTransport();
            PX_RELEASE(psPvd)
            PX_RELEASE(transport)
        }
        PX_RELEASE(psFoundation)
    }
    PhysicsPhysX::~PhysicsPhysX()
    {
        CleanupPhysics();
    }
    
    
    
    /// --- Rigidbody Part --- ///
    RigidbodyPhysX::RigidbodyPhysX(const Transform& rbTransform)
    {
        const PxTransform transform {
            PxVec3(rbTransform.GetWorldPosition().x, rbTransform.GetWorldPosition().y, rbTransform.GetWorldPosition().z),
            PxQuat(rbTransform.GetWorldRotation().x, rbTransform.GetWorldRotation().y, rbTransform.GetWorldRotation().z, rbTransform.GetWorldRotation().w)
        };
        rbActor = PhysicsPhysX::GetInstance().GetPhysics()->createRigidDynamic(transform);
        PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<PxRigidDynamic*>(rbActor)), 10);
        PhysicsPhysX::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidDynamic*>(rbActor)));
    }
    
    void RigidbodyPhysX::RemoveActor()
    {
        PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
        const PxU32 nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        for(PxU32 i = 0; i < nbShapes; ++i)
        {
            rbActor->detachShape(*shapes[i]);
        }
        PhysicsPhysX::GetInstance().GetScene()->removeActor(*rbActor);
    }
    
    void RigidbodyPhysX::SetActorType(PxActorType::Enum requestedActorType)
    {
        if (rbActor->getType() == requestedActorType) { return; }
        
        RemoveActor();
        
        switch (requestedActorType)
        {
        case PxActorType::eRIGID_STATIC:
            rbActor = PhysicsPhysX::GetInstance().GetPhysics()->createRigidStatic(rbActor->getGlobalPose());
            PhysicsPhysX::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidStatic*>(rbActor)));
            break;
        case PxActorType::eRIGID_DYNAMIC:
            rbActor = PhysicsPhysX::GetInstance().GetPhysics()->createRigidDynamic(rbActor->getGlobalPose());
            PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<PxRigidDynamic*>(rbActor)), 10);
            PhysicsPhysX::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidDynamic*>(rbActor)));
            break;
        default:
            throw "<ERROR> void RigidbodyPhysX::SetActorType(PxActorType::Enum requestedActorType) <ERROR>";
        }
    }

    void RigidbodyPhysX::AttachShapeGeometry(PxGeometryType::Enum requestedGeometryType)
    {
        if (rbActor->getNbShapes() >= MAX_NUM_ACTOR_SHAPES) { return; }
        
        const auto material = PhysicsPhysX::GetInstance().GetPhysics()->createMaterial(0.5f, 0.5f, 0.6f);
        PxShape* shape = nullptr;
        switch (requestedGeometryType)
        {
        case PxGeometryType::ePLANE:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxPlaneGeometry(), *material, true);
            break;
        case PxGeometryType::eBOX:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxBoxGeometry(0.5f, 0.5f, 0.5f), *material, true);
            break;
        case PxGeometryType::eSPHERE:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxSphereGeometry(0.5f), *material, true);
            break;
        case PxGeometryType::eCAPSULE:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxCapsuleGeometry(0.5f, 0.5f), *material, true);
            break;
        default:
            throw "<ERROR> void RigidbodyPhysX::AttachShapeGeometry(PxGeometryType::Enum requestedGeometryType) <ERROR>";
        }
        rbActor->attachShape(*shape);
        shape->release();
        material->release();
    }
    
    void RigidbodyPhysX::DetachShapeGeometry(PxU32 sequenceShapeNumber)
    {
        PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
        const PxU32 nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        rbActor->detachShape(*shapes[sequenceShapeNumber]);
    }
    
    PxVec3 RigidbodyPhysX::GetShapeGeometryParams(PxU32 sequenceShapeNumber)
    {
        PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
        const PxU32 nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        switch (shapes[sequenceShapeNumber]->getGeometry().getType())
        {
        case PxGeometryType::eBOX:
            return PxVec3(reinterpret_cast<PxBoxGeometry*>(shapes[sequenceShapeNumber])->halfExtents);
        case PxGeometryType::eSPHERE:
            return PxVec3(reinterpret_cast<PxSphereGeometry*>(shapes[sequenceShapeNumber])->radius, 0, 0);
        case PxGeometryType::eCAPSULE:
            return PxVec3(reinterpret_cast<PxCapsuleGeometry*>(shapes[sequenceShapeNumber])->radius, reinterpret_cast<PxCapsuleGeometry*>(shapes[sequenceShapeNumber])->halfHeight, 0);
        default:
            throw "<ERROR> PxVec3 RigidbodyPhysX::GetShapeGeometryParams(PxU32 sequenceShapeNumber) <ERROR>";
        }
    }
    
    void RigidbodyPhysX::SetShapeGeometryParam(PxU32 sequenceShapeNumber, PxU32 sequenceParamNumber, PxReal param)
    {
        PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
        const PxU32 nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        
        const PxGeometryHolder geom(shapes[sequenceShapeNumber]->getGeometry());
        PxReal initialX {}, initialY {}, initialZ {}, initialRadius {}, initialHeight {};
        switch (shapes[sequenceShapeNumber]->getGeometry().getType())
        {
        case PxGeometryType::eBOX:
            initialX = geom.box().halfExtents.x; initialY = geom.box().halfExtents.y; initialZ = geom.box().halfExtents.z;
            if (sequenceParamNumber == 0) { shapes[sequenceShapeNumber]->setGeometry(PxBoxGeometry(param, initialY, initialZ)); }
            if (sequenceParamNumber == 1) { shapes[sequenceShapeNumber]->setGeometry(PxBoxGeometry(initialX, param, initialZ)); }
            if (sequenceParamNumber == 2) { shapes[sequenceShapeNumber]->setGeometry(PxBoxGeometry(initialX, initialY, param)); }
            break;
        case PxGeometryType::eSPHERE:
            if (sequenceParamNumber == 0) { shapes[sequenceShapeNumber]->setGeometry(PxSphereGeometry(param)); }
            break;
        case PxGeometryType::eCAPSULE:
            initialRadius = geom.capsule().radius; initialHeight = geom.capsule().halfHeight;
            if (sequenceParamNumber == 0) { shapes[sequenceShapeNumber]->setGeometry(PxCapsuleGeometry(param, initialHeight));    }
            if (sequenceParamNumber == 1) { shapes[sequenceShapeNumber]->setGeometry(PxBoxGeometry(initialRadius, param)); }
            break;
        default:
            throw "<ERROR> void RigidbodyPhysX::SetShapeGeometryParam(PxU32 sequenceShapeNumber, PxU32 sequenceParamNumber, PxReal param) <ERROR>";
        }
    }

    RigidbodyPhysX::~RigidbodyPhysX()
    {
        PX_RELEASE(rbActor)
    }

    // Actor
    bool RigidbodyPhysX::GetActorGravityStatus()
    {
        return rbActor->getActorFlags().isSet(PxActorFlag::eDISABLE_GRAVITY);
    }
    bool RigidbodyPhysX::GetShapeGeometryVisualizationStatus()
    {
        return rbActor->getActorFlags().isSet(PxActorFlag::eVISUALIZATION);
    }
    PxReal RigidbodyPhysX::GetActorMass()
    {
        if (rbActor->getType() == PxActorType::eRIGID_STATIC) { return 0; }
        return reinterpret_cast<PxRigidDynamic*>(rbActor)->getMass();
    }
    PxVec3 RigidbodyPhysX::GetActorLinearVelocity()
    {
        if (rbActor->getType() == PxActorType::eRIGID_STATIC) { return PxVec3(0,0,0); }
        return reinterpret_cast<PxRigidDynamic*>(rbActor)->getLinearVelocity();
    }
    PxVec3 RigidbodyPhysX::GetActorAngularVelocity()
    {
        if (rbActor->getType() == PxActorType::eRIGID_STATIC) { return PxVec3(0,0,0); }
        return reinterpret_cast<PxRigidDynamic*>(rbActor)->getAngularVelocity();
    }
    void RigidbodyPhysX::SetActorGravityStatus(const bool status)
    {
        rbActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, status);
    }
    void RigidbodyPhysX::SetActorShapeGeometryVisualizationStatus(const bool status)
    {
        rbActor->setActorFlag(PxActorFlag::eVISUALIZATION, status);
    }
    void RigidbodyPhysX::SetActorMass(PxReal mass)
    {
        if (rbActor->getType() == PxActorType::eRIGID_STATIC) { return; }
        reinterpret_cast<PxRigidDynamic*>(rbActor)->setMass(mass);
    }
    void RigidbodyPhysX::SetActorLinearVelocity(PxVec3 linearVelocity)
    {
        if (rbActor->getType() == PxActorType::eRIGID_STATIC) { return; }
        reinterpret_cast<PxRigidDynamic*>(rbActor)->setLinearVelocity(linearVelocity);
    }
    void RigidbodyPhysX::SetActorAngularVelocity(PxVec3 angularVelocity)
    {
        if (rbActor->getType() == PxActorType::eRIGID_STATIC) { return; }
        reinterpret_cast<PxRigidDynamic*>(rbActor)->setAngularVelocity(angularVelocity);
    }
    
    // Material
    PxReal RigidbodyPhysX::GetStaticFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber)
    {
        PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
        const PxU32 nbShapes = rbActor->getNbShapes();
        if (nbShapes <= 0) { return 0; }
        rbActor->getShapes(shapes, nbShapes);

        PxMaterial* materials[MAX_NUM_ACTOR_MATERIALS];
        const PxU32 nbMaterials = shapes[sequenceShapeNumber]->getNbMaterials();
        if (nbMaterials <= 0) { return 0; }
        
        shapes[sequenceShapeNumber]->getMaterials(materials, nbMaterials);
        return materials[sequenceMaterialNumber]->getStaticFriction();
    }
    PxReal RigidbodyPhysX::GetDynamicFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber)
    {
        PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
        const PxU32 nbShapes = rbActor->getNbShapes();
        if (nbShapes <= 0) { return 0; }
        rbActor->getShapes(shapes, nbShapes);

        PxMaterial* materials[MAX_NUM_ACTOR_MATERIALS];
        const PxU32 nbMaterials = shapes[sequenceShapeNumber]->getNbMaterials();
        if (nbMaterials <= 0) { return 0; }
        
        shapes[sequenceShapeNumber]->getMaterials(materials, nbMaterials);
        return materials[sequenceMaterialNumber]->getDynamicFriction();
    }
    PxReal RigidbodyPhysX::GetRestitution(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber)
    {
        PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
        const PxU32 nbShapes = rbActor->getNbShapes();
        if (nbShapes <= 0) { return 0; }
        rbActor->getShapes(shapes, nbShapes);

        PxMaterial* materials[MAX_NUM_ACTOR_MATERIALS];
        const PxU32 nbMaterials = shapes[sequenceShapeNumber]->getNbMaterials();
        if (nbMaterials <= 0) { return 0; }
        
        shapes[sequenceShapeNumber]->getMaterials(materials, nbMaterials);
        return materials[sequenceMaterialNumber]->getRestitution();
    }
    void RigidbodyPhysX::SetStaticFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal staticFriction)
    {
        if ((staticFriction < 0.0f) && (staticFriction > PX_MAX_F32)) { return; }
        
        PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
        const PxU32 nbShapes = rbActor->getNbShapes();
        if (nbShapes <= 0) { return; }
        rbActor->getShapes(shapes, nbShapes);

        PxMaterial* materials[MAX_NUM_ACTOR_MATERIALS];
        const PxU32 nbMaterials = shapes[sequenceShapeNumber]->getNbMaterials();
        if (nbMaterials <= 0) { return; }
        
        shapes[sequenceShapeNumber]->getMaterials(materials, nbMaterials);
        materials[sequenceMaterialNumber]->setStaticFriction(staticFriction);
    }
    void RigidbodyPhysX::SetDynamicFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal dynamicFriction)
    {
        if ((dynamicFriction < 0.0f) && (dynamicFriction > PX_MAX_F32)) { return; }
        
        PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
        const PxU32 nbShapes = rbActor->getNbShapes();
        if (nbShapes <= 0) { return; }
        rbActor->getShapes(shapes, nbShapes);

        PxMaterial* materials[MAX_NUM_ACTOR_MATERIALS];
        const PxU32 nbMaterials = shapes[sequenceShapeNumber]->getNbMaterials();
        if (nbMaterials <= 0) { return; }
        
        shapes[sequenceShapeNumber]->getMaterials(materials, nbMaterials);
        materials[sequenceMaterialNumber]->setDynamicFriction(dynamicFriction);
    }
    void RigidbodyPhysX::SetRestitution(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal restitution)
    {
        if ((restitution < 0.0f) && (restitution > 1.0f)) { return; }
        
        PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
        const PxU32 nbShapes = rbActor->getNbShapes();
        if (nbShapes <= 0) { return; }
        rbActor->getShapes(shapes, nbShapes);

        PxMaterial* materials[MAX_NUM_ACTOR_MATERIALS];
        const PxU32 nbMaterials = shapes[sequenceShapeNumber]->getNbMaterials();
        if (nbMaterials <= 0) { return; }
        
        shapes[sequenceShapeNumber]->getMaterials(materials, nbMaterials);
        materials[sequenceMaterialNumber]->setRestitution(restitution);
    }



    /*
    /// --- ImGui Interface Part --- ///
    class RigidbodyPhysXSerializer final : public ComponentSerializer<RigidbodyPhysX, RigidbodyPhysXSerializer>
    {
    protected:
        const char* GetKey() override { return "RigidbodyPhysX"; }

        void SerializeData(YAML::Emitter& out, const RigidbodyPhysX& component) override
        {
            out << YAML::Key << "Body Type" << YAML::Value << static_cast<int>(component.bodyType);
            out << YAML::Key << "Gravity Scale" << YAML::Value << component.gravityScale;
            out << YAML::Key << "Mass" << YAML::Value << component.mass;
            out << YAML::Key << "Elasticity" << YAML::Value << component.elasticity;
            out << YAML::Key << "Damping" << YAML::Value << component.damping;
            out << YAML::Key << "Angular Damping" << YAML::Value << component.angularDamping;
            out << YAML::Key << "Enabled" << YAML::Value << component.enabled;
        }

        void DeserializeData(YAML::Node& node, Rigidbody& component) override
        {
            component.bodyType = static_cast<BodyType>(node["Body Type"].as<int>());
            component.gravityScale = node["Gravity Scale"].as<float>();
            component.mass = node["Mass"].as<float>();
            component.elasticity = node["Elasticity"].as<float>();
            component.damping = node["Damping"].as<float>();
            component.angularDamping = node["Angular Damping"].as<float>();
            component.enabled = node["Enabled"].as<bool>();
        }
    };
    */
}

