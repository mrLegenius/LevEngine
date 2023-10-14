#include "levpch.h"
#include "PhysicsPhysX.h"
#include "Renderer/DebugRender/DebugRender.h"

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
                const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *rigidbody.GetActor()));
                float array[9] = {
                    shapePose.column0.x, shapePose.column0.y, shapePose.column0.z,
                    shapePose.column1.x, shapePose.column1.y, shapePose.column1.z,
                    shapePose.column2.x, shapePose.column2.y, shapePose.column2.z
                };
                PxQuat quat = PxQuat
                (
                    PxMat33
                    (
                        array
                    )
                );
                transform.SetWorldRotation
                (
                    Quaternion
                    (
                        quat.x,
                        quat.y,
                        quat.z,
                        quat.w
                    )
                );
                transform.SetWorldPosition
                (
                    Vector3
                    (
                        shapePose.getPosition().x,
                        shapePose.getPosition().y,
                        shapePose.getPosition().z
                    )
                );
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
        //DrawDebugLines();
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

    
    
    /// -- Rigidbody Part -- ///
    RigidbodyPhysX::RigidbodyPhysX(const Transform& rbTransform)
    {
        const auto& matrix = Matrix::CreateFromQuaternion(rbTransform.GetWorldRotation()) * Matrix::CreateTranslation(rbTransform.GetWorldPosition());
        float array[16] = {
            matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
            matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
            matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
            matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]
        };
        rbActor = PhysicsPhysX::GetInstance().GetPhysics()->createRigidDynamic(PxTransform(PxMat44(array)));
        PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<PxRigidDynamic*>(rbActor)), 10);
        PhysicsPhysX::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidDynamic*>(rbActor)));
    }

    void RigidbodyPhysX::CleanupActor()
    {
        PxShape* shapes[128];
        const PxU32 nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        for(PxU32 j = 0; j < nbShapes; ++j)
        {
            rbActor->detachShape(*shapes[j]);
        }
        PhysicsPhysX::GetInstance().GetScene()->removeActor(*rbActor);
    }
    
    void RigidbodyPhysX::SetActorType(PxActorType::Enum requestedActorType)
    {
        if (rbActor->getType() == requestedActorType) { return; }

        const auto pose = rbActor->getGlobalPose();
        
        CleanupActor();
        
        switch (requestedActorType)
        {
        case PxActorType::eRIGID_STATIC:
            rbActor = PhysicsPhysX::GetInstance().GetPhysics()->createRigidStatic(pose);
            PhysicsPhysX::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidStatic*>(rbActor)));
            break;
        case PxActorType::eRIGID_DYNAMIC:
            rbActor = PhysicsPhysX::GetInstance().GetPhysics()->createRigidDynamic(pose);
            PxRigidBodyExt::updateMassAndInertia(*(reinterpret_cast<PxRigidDynamic*>(rbActor)), 10);
            PhysicsPhysX::GetInstance().GetScene()->addActor(*(reinterpret_cast<PxRigidDynamic*>(rbActor)));
            break;
        default:
            break;
        }
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

    void RigidbodyPhysX::AttachShapeGeometry(PxGeometryType::Enum requestedGeometryType)
    {
        if ((rbActor->getType() == PxActorType::eRIGID_DYNAMIC) && (requestedGeometryType == PxGeometryType::ePLANE)) { return; }
        if ((rbActor->getNbShapes() > 128)) { return; }
        
        const auto material = PhysicsPhysX::GetInstance().GetPhysics()->createMaterial(0.5f, 0.5f, 0.6f);
        PxShape* shape = nullptr;
        switch (requestedGeometryType)
        {
        case PxGeometryType::ePLANE:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxPlaneGeometry(), *material, true);
            rbActor->attachShape(*shape);
            shape->release();
            break;
        case PxGeometryType::eBOX:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxBoxGeometry(0.5f, 0.5f, 0.5f), *material, true);
            rbActor->attachShape(*shape);
            shape->release();
            break;
        case PxGeometryType::eSPHERE:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxSphereGeometry(0.5f), *material, true);
            rbActor->attachShape(*shape);
            shape->release();
            break;
        case PxGeometryType::eCAPSULE:
            shape = PhysicsPhysX::GetInstance().GetPhysics()->createShape(PxCapsuleGeometry(0.5f, 0.5f), *material, true);
            rbActor->attachShape(*shape);
            shape->release();
            break;
        default:
            throw "<ERROR> void RigidbodyPhysX::AddShape(PxGeometryType::Enum requestedGeometryType) <ERROR>";
        }
        material->release();
    }
    
    void RigidbodyPhysX::DetachShapeGeometryType(PxU32 sequenceShapeNumber)
    {
        PxShape* shapes[128];
        const PxU32 nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        rbActor->detachShape(*shapes[sequenceShapeNumber]);
    }

    void RigidbodyPhysX::SetStaticFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal staticFriction)
    {
        if ((staticFriction < 0.0f) && (staticFriction > PX_MAX_F32)) { return; }
        
        PxShape* shapes[128];
        const PxU32 nbShapes = rbActor->getNbShapes();
        if (nbShapes <= 0) { return; }
        rbActor->getShapes(shapes, nbShapes);

        PxMaterial* materials[128];
        const PxU32 nbMaterials = shapes[sequenceShapeNumber]->getNbMaterials();
        if (nbMaterials <= 0) { return; }
        
        shapes[sequenceShapeNumber]->getMaterials(materials, nbMaterials);
        materials[sequenceMaterialNumber]->setStaticFriction(staticFriction);
    }
    void RigidbodyPhysX::SetDynamicFriction(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal dynamicFriction)
    {
        if ((dynamicFriction < 0.0f) && (dynamicFriction > PX_MAX_F32)) { return; }
        
        PxShape* shapes[128];
        const PxU32 nbShapes = rbActor->getNbShapes();
        if (nbShapes <= 0) { return; }
        rbActor->getShapes(shapes, nbShapes);

        PxMaterial* materials[128];
        const PxU32 nbMaterials = shapes[sequenceShapeNumber]->getNbMaterials();
        if (nbMaterials <= 0) { return; }
        
        shapes[sequenceShapeNumber]->getMaterials(materials, nbMaterials);
        materials[sequenceMaterialNumber]->setDynamicFriction(dynamicFriction);
    }
    void RigidbodyPhysX::SetRestitution(PxU32 sequenceShapeNumber, PxU32 sequenceMaterialNumber, PxReal restitution)
    {
        if ((restitution < 0.0f) && (restitution > 1.0f)) { return; }
        
        PxShape* shapes[128];
        const PxU32 nbShapes = rbActor->getNbShapes();
        if (nbShapes <= 0) { return; }
        rbActor->getShapes(shapes, nbShapes);

        PxMaterial* materials[128];
        const PxU32 nbMaterials = shapes[sequenceShapeNumber]->getNbMaterials();
        if (nbMaterials <= 0) { return; }
        
        shapes[sequenceShapeNumber]->getMaterials(materials, nbMaterials);
        materials[sequenceMaterialNumber]->setRestitution(restitution);
    }

    
    void RigidbodyPhysX::SetShapeGeometryParams(PxU32 sequenceShapeNumber, PxReal param1, PxReal param2, PxReal param3)
    {
        PxShape* shapes[128];
        const PxU32 nbShapes = rbActor->getNbShapes();
        rbActor->getShapes(shapes, nbShapes);
        if (!nbShapes > 0) return;
        if (!shapes[sequenceShapeNumber]->isExclusive()) return;
        
        switch (shapes[sequenceShapeNumber]->getGeometry().getType())
        {
        case PxGeometryType::ePLANE:
            shapes[sequenceShapeNumber]->setGeometry(PxPlaneGeometry());
            break;
        case PxGeometryType::eBOX:
            shapes[sequenceShapeNumber]->setGeometry(PxBoxGeometry(param1, param2, param3));
            break;
        case PxGeometryType::eSPHERE:
            shapes[sequenceShapeNumber]->setGeometry(PxSphereGeometry(param1));
            break;
        case PxGeometryType::eCAPSULE:
            shapes[sequenceShapeNumber]->setGeometry(PxCapsuleGeometry(param1, param2));
            break;
        default:
            throw "<ERROR> void RigidbodyPhysX::ChangeShapeParams(PxU32 sequenceShapeNumber, float param0, float param1, float param2) <ERROR>";
        }
    }
    
    RigidbodyPhysX::~RigidbodyPhysX()
    {
        PX_RELEASE(rbActor)
    }
}

