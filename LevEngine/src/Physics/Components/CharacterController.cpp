#include "levpch.h"
#include "CharacterController.h"

#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsUtils.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    void CharacterController::OnDestroy(entt::registry& registry, entt::entity entity)
    {
        auto [transform, controller] = registry.get<Transform, CharacterController>(entity);
        if (controller.GetController() != NULL)
        {
            controller.DetachController();
        }
    }
    
    bool CharacterController::IsInitialized() const
    {
        return m_IsInitialized;
    }

    void CharacterController::Initialize(const Entity entity)
    {
        if (m_IsInitialized) return;
        
        AttachController(entity);
        
        m_IsInitialized = true;

        const auto& transform = entity.GetComponent<Transform>();
        SetTransformScale(transform.GetWorldScale());
    }

    physx::PxController* CharacterController::GetController() const
    {
        return m_Controller;
    }

    physx::PxRigidActor* CharacterController::GetActor() const
    {
        return m_Controller->getActor();
    }

    physx::PxShape* CharacterController::GetCollider() const
    {
        physx::PxShape* collider[1];
        const auto nbColliders = GetActor()->getNbShapes();
        GetActor()->getShapes(collider, nbColliders);

        return *collider;
    }

    physx::PxMaterial* CharacterController::GetPhysicalMaterial() const
    {
        physx::PxMaterial* physicalMaterial[1];
        const auto nbMaterials = GetCollider()->getNbMaterials();
        GetCollider()->getMaterials(physicalMaterial, nbMaterials);

        return *physicalMaterial;
    }

    Vector3 CharacterController::GetTransformScale() const
    {
        return m_TransformScale;
    }

    void CharacterController::SetTransformScale(const Vector3 transformScale)
    {
        if (m_TransformScale != transformScale)
        {
            m_TransformScale = transformScale;
        }

        if (m_IsInitialized)
        {
            switch (GetControllerType())
            {
            case Controller::Type::BoxController:
                SetBoxControllerHalfHeight(GetBoxControllerHalfHeight());
                SetBoxControllerHalfSideExtent(GetBoxControllerHalfSideExtent());
                SetBoxControllerHalfForwardExtent(GetBoxControllerHalfForwardExtent());
                break;
            case Controller::Type::CapsuleController:
                SetCapsuleControllerRadius(GetCapsuleControllerRadius());
                SetCapsuleControllerHalfHeight(GetCapsuleControllerHalfHeight());
                SetCapsuleControllerClimbingMode(GetCapsuleControllerClimbingMode());
                break;
            default:
                break;
            }
        }
    }

    bool CharacterController::IsVisualizationEnabled() const
    {
        return m_IsVisualizationEnabled;
    }
    
    void CharacterController::EnableVisualization(const bool flag)
    {
        m_IsVisualizationEnabled = flag;
        
        if (m_Controller != NULL)
        {
            GetActor()->setActorFlag(physx::PxActorFlag::eVISUALIZATION, flag);
        }
    }

    Controller::Type CharacterController::GetControllerType() const
    {
        return m_CharacterController->m_Type;
    }

    void CharacterController::SetControllerType(const Controller::Type& type)
    {
        switch (type)
        {
        case Controller::Type::BoxController:
            m_CharacterController = CreateRef<BoxController>();
            break;
        case Controller::Type::CapsuleController:
            m_CharacterController = CreateRef<CapsuleController>();
            break;
        default:
            break;
        }
        m_CharacterController->m_Type = type;
        
        if (m_Controller != NULL)
        {
            // copy old Entity
            const auto entity = App::Get().GetPhysics().m_ActorEntityMap.at(GetActor());
            
            AttachController(entity);
            SetTransformScale(m_TransformScale);
        }
    }

    float CharacterController::GetSlopeLimit() const
    {
        return m_CharacterController->SlopeLimit;
    }

    void CharacterController::SetSlopeLimit(const float slopeLimit) const
    {
        if (slopeLimit < 0.0f) return;
        
        m_CharacterController->SlopeLimit = slopeLimit;

        if (m_Controller != NULL)
        {
            m_Controller->setSlopeLimit(slopeLimit);
        }
    }

    float CharacterController::GetStepOffset() const
    {
        return m_CharacterController->StepOffset;
    }

    void CharacterController::SetStepOffset(const float stepOffset) const
    {
        if (stepOffset < 0.0f) return;
        
        m_CharacterController->StepOffset = stepOffset;

        if (m_Controller != NULL)
        {
            m_Controller->setStepOffset(stepOffset);
        }
    }

    float CharacterController::GetContactOffset() const
    {
        return m_CharacterController->ContactOffset;
    }

    void CharacterController::SetContactOffset(const float contactOffset) const
    {
        if (contactOffset <= 0.0f) return;
        
        m_CharacterController->ContactOffset = contactOffset;

        if (m_Controller != NULL)
        {
            m_Controller->setContactOffset(contactOffset);
        }
    }

    float CharacterController::GetMinimumMovementDistance() const
    {
        return m_CharacterController->MinimumMovementDistance;
    }

    void CharacterController::SetMinimumMovementDistance(const float minimumMovementDistance) const
    {
        if (minimumMovementDistance < 0.0f) return;
        
        m_CharacterController->MinimumMovementDistance = minimumMovementDistance;
    }

    Controller::NonWalkableMode CharacterController::GetNonWalkableMode() const
    {
        return m_CharacterController->m_NonWalkableMode;
    }

    void CharacterController::SetNonWalkableMode(const Controller::NonWalkableMode& nonWalkableMode) const
    {
        m_CharacterController->m_NonWalkableMode = nonWalkableMode;

        if (m_Controller != NULL)
        {
            switch (nonWalkableMode)
            {
            case Controller::NonWalkableMode::PreventClimbing:
                m_Controller->setNonWalkableMode(physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING);
                break;
            case Controller::NonWalkableMode::PreventClimbingAndForceSliding:
                m_Controller->setNonWalkableMode(physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING);
                break;
            default:
                break;
            }
        }
    }

    Vector3 CharacterController::GetControllerCenter() const
    {
        return m_CharacterController->Center;
    }

    void CharacterController::SetControllerCenter(const Vector3 center) const
    {
        if (center.x < 0.0f || center.y < 0.0f || center.z < 0.0f) return;
        
        m_CharacterController->Center = center;

        if (m_Controller != NULL)
        {
            const auto collider = GetCollider();
            const auto rotation = collider->getLocalPose().q;
            collider->setLocalPose(physx::PxTransform(PhysicsUtils::FromVector3ToPxVec3(center), rotation));
        }
    }

    float CharacterController::GetBoxControllerHalfHeight() const
    {
        return static_cast<BoxController*>(m_CharacterController.get())->HalfHeight;
    }

    void CharacterController::SetBoxControllerHalfHeight(const float halfHeight) const
    {
        if (halfHeight <= 0.0f) return;

        if (halfHeight * 2.0f < GetStepOffset()) return;
        
        static_cast<BoxController*>(m_CharacterController.get())->HalfHeight = halfHeight;

        if (m_Controller != NULL)
        {
            const float maxTransformScale = Math::MaxElement(m_TransformScale);

            const auto collider = GetCollider();
            const physx::PxGeometryHolder geom(collider->getGeometry());
            const auto initialHalfSideExtent = geom.box().halfExtents.x;
            const auto initialHalfForwardExtent = geom.box().halfExtents.z;
            collider->setGeometry(
                physx::PxBoxGeometry(
                    PhysicsUtils::FromVector3ToPxVec3(
                        Vector3(
                            initialHalfSideExtent,
                            halfHeight * maxTransformScale,
                            initialHalfForwardExtent
                        )
                    )
                )
            );
        }
    }
    
    float CharacterController::GetBoxControllerHalfSideExtent() const
    {
        return static_cast<BoxController*>(m_CharacterController.get())->HalfSideExtent;
    }

    void CharacterController::SetBoxControllerHalfSideExtent(const float halfSideExtent) const
    {
        if (halfSideExtent <= 0.0f) return;
        
        static_cast<BoxController*>(m_CharacterController.get())->HalfSideExtent = halfSideExtent;

        if (m_Controller != NULL)
        {
            const float maxTransformScale = Math::MaxElement(m_TransformScale);

            const auto collider = GetCollider();
            const physx::PxGeometryHolder geom(collider->getGeometry());
            const auto initialHalfHeight = geom.box().halfExtents.y;
            const auto initialHalfForwardExtent = geom.box().halfExtents.z;
            collider->setGeometry(
                physx::PxBoxGeometry(
                    PhysicsUtils::FromVector3ToPxVec3(
                        Vector3(
                            halfSideExtent * maxTransformScale,
                            initialHalfHeight,
                            initialHalfForwardExtent
                        )
                    )
                )
            );
        }
    }

    float CharacterController::GetBoxControllerHalfForwardExtent() const
    {
        return static_cast<BoxController*>(m_CharacterController.get())->HalfForwardExtent;
    }

    void CharacterController::SetBoxControllerHalfForwardExtent(const float halfForwardExtent) const
    {
        if (halfForwardExtent <= 0.0f) return;
        
        static_cast<BoxController*>(m_CharacterController.get())->HalfForwardExtent = halfForwardExtent;

        if (m_Controller != NULL)
        {
            const float maxTransformScale = Math::MaxElement(m_TransformScale);

            const auto collider = GetCollider();
            const physx::PxGeometryHolder geom(collider->getGeometry());
            const auto initialHalfSideExtent = geom.box().halfExtents.x;
            const auto initialHalfHeight = geom.box().halfExtents.y;
            collider->setGeometry(
                physx::PxBoxGeometry(
                    PhysicsUtils::FromVector3ToPxVec3(
                        Vector3(
                            initialHalfSideExtent,
                            initialHalfHeight,
                            halfForwardExtent * maxTransformScale
                        )
                    )
                )
            );
        }
    }

    float CharacterController::GetCapsuleControllerRadius() const
    {
        return static_cast<CapsuleController*>(m_CharacterController.get())->Radius;
    }

    void CharacterController::SetCapsuleControllerRadius(const float radius) const
    {
        if (radius <= 0.0f) return;
        
        static_cast<CapsuleController*>(m_CharacterController.get())->Radius = radius;

        if (m_Controller != NULL)
        {
            const float maxTransformScale = Math::MaxElement(m_TransformScale);

            const auto collider = GetCollider();
            const physx::PxGeometryHolder geom(collider->getGeometry());
            const auto initialHeight = geom.capsule().halfHeight;
            collider->setGeometry(physx::PxCapsuleGeometry(radius * maxTransformScale, initialHeight));
        }
    }

    float CharacterController::GetCapsuleControllerHalfHeight() const
    {
        return static_cast<CapsuleController*>(m_CharacterController.get())->HalfHeight;
    }

    void CharacterController::SetCapsuleControllerHalfHeight(const float halfHeight) const
    {
        if (halfHeight <= 0.0f) return;
        
        static_cast<CapsuleController*>(m_CharacterController.get())->HalfHeight = halfHeight;

        if (m_Controller != NULL)
        {
            const float maxTransformScale = Math::MaxElement(m_TransformScale);
            
            const auto collider = GetCollider();
            const physx::PxGeometryHolder geom(collider->getGeometry());
            const auto initialRadius = geom.capsule().radius;
            collider->setGeometry(physx::PxCapsuleGeometry(initialRadius, halfHeight * maxTransformScale));
        }
    }

    CapsuleController::ClimbingMode CharacterController::GetCapsuleControllerClimbingMode() const
    {
        return static_cast<CapsuleController*>(m_CharacterController.get())->m_ClimbingMode;
    }

    void CharacterController::SetCapsuleControllerClimbingMode(const CapsuleController::ClimbingMode& climbingMode) const
    {
        static_cast<CapsuleController*>(m_CharacterController.get())->m_ClimbingMode = climbingMode;

        if (m_Controller != NULL)
        {
            switch (climbingMode)
            {
            case CapsuleController::ClimbingMode::Easy:
                static_cast<physx::PxCapsuleController*>(m_Controller)->setClimbingMode(physx::PxCapsuleClimbingMode::eEASY);
                break;
            case CapsuleController::ClimbingMode::Constrained:
                static_cast<physx::PxCapsuleController*>(m_Controller)->setClimbingMode(physx::PxCapsuleClimbingMode::eCONSTRAINED);
                break;
            default:
                break;
            }
        }
    }

    float CharacterController::GetStaticFriction() const
    {
        return m_CharacterController->m_PhysicalMaterial->StaticFriction;
    }

    void CharacterController::SetStaticFriction(const float staticFriction) const
    {
        if (staticFriction < 0.0f || staticFriction > 1.0f) return;
        
        m_CharacterController->m_PhysicalMaterial->StaticFriction = staticFriction;
        
        if (m_Controller != NULL)
        {
            const auto physicalMaterials = GetPhysicalMaterial();
            physicalMaterials[0].setStaticFriction(staticFriction);
        }
    }
    
    float CharacterController::GetDynamicFriction() const
    {
        return m_CharacterController->m_PhysicalMaterial->DynamicFriction;
    }

    void CharacterController::SetDynamicFriction(const float dynamicFriction) const
    {
        if (dynamicFriction < 0.0f || dynamicFriction > 1.0f) return;
        
        m_CharacterController->m_PhysicalMaterial->DynamicFriction = dynamicFriction;
        
        if (m_Controller != NULL)
        {
            const auto physicalMaterials = GetPhysicalMaterial();
            physicalMaterials[0].setDynamicFriction(dynamicFriction);
        }
    }
    
    float CharacterController::GetRestitution() const
    {
        return m_CharacterController->m_PhysicalMaterial->Restitution;
    }

    void CharacterController::SetRestitution(const float restitution) const
    {
        if (restitution < 0.0f || restitution > 1.0f) return;
        
        m_CharacterController->m_PhysicalMaterial->Restitution = restitution;
        
        if (m_Controller != NULL)
        {
            const auto physicalMaterials = GetPhysicalMaterial();
            physicalMaterials[0].setRestitution(restitution);
        }
    }

    void CharacterController::MoveForward()
    {
        if (m_Controller != NULL)
        {
            m_Controller->move(
                
            );
        }
    }

    void CharacterController::MoveBackward()
    {
        
    }

    void CharacterController::MoveRight()
    {
        
    }

    void CharacterController::MoveLeft()
    {
        
    }

    void CharacterController::AttachController(const Entity entity)
    {
        if (m_Controller != NULL)
        {
            DetachController();
        }
        
        const auto physicalMaterial =
            App::Get().GetPhysics().CreateMaterial(
                GetStaticFriction(),
                GetDynamicFriction(),
                GetRestitution()
            );

        switch (GetControllerType())
        {
        case Controller::Type::BoxController:
            m_Controller =
                App::Get().GetPhysics().CreateBoxController(
                    entity,
                    GetBoxControllerHalfHeight(),
                    GetBoxControllerHalfSideExtent(),
                    GetBoxControllerHalfForwardExtent(),
                    physicalMaterial
                );
            break;
        case Controller::Type::CapsuleController:
            m_Controller =
                App::Get().GetPhysics().CreateCapsuleController(
                    entity,
                    GetCapsuleControllerRadius(),
                    GetCapsuleControllerHalfHeight(),
                    GetCapsuleControllerClimbingMode(),
                    physicalMaterial
                );
            break;
        default:
            break;
        }

        physicalMaterial->release();

        EnableVisualization(IsVisualizationEnabled());
        
        SetControllerCenter(GetControllerCenter());
        
        SetSlopeLimit(GetSlopeLimit());
        SetStepOffset(GetStepOffset());
        SetContactOffset(GetContactOffset());
        SetMinimumMovementDistance(GetMinimumMovementDistance());
        SetNonWalkableMode(GetNonWalkableMode());
    }

    void CharacterController::DetachController() const
    {
        App::Get().GetPhysics().RemoveController(m_Controller);
    }

    class CharacterControllerSerializer final : public ComponentSerializer<CharacterController, CharacterControllerSerializer>
    {
    protected:
        const char* GetKey() override { return "Character Controller"; }

        void SerializeData(YAML::Emitter& out, const CharacterController& component) override
        {
            out << YAML::Key << "Is Visualization Enabled" << YAML::Value << component.IsVisualizationEnabled();
            
            out << YAML::Key << "Controller Type" << YAML::Value << static_cast<int>(component.GetControllerType());
            switch (component.GetControllerType())
            {
            case Controller::Type::BoxController:
                out << YAML::Key << "Box Controller Half Height" << YAML::Value << component.GetBoxControllerHalfHeight();
                out << YAML::Key << "Box Controller Half Side Extent" << YAML::Value << component.GetBoxControllerHalfSideExtent();
                out << YAML::Key << "Box Controller Half Forward Extent" << YAML::Value << component.GetBoxControllerHalfForwardExtent();
                break;
            case Controller::Type::CapsuleController:
                out << YAML::Key << "Capsule Controller Radius" << YAML::Value << component.GetCapsuleControllerRadius();
                out << YAML::Key << "Capsule Controller Half Height" << YAML::Value << component.GetCapsuleControllerHalfHeight();
                out << YAML::Key << "Capsule Controller Climbing Mode" << YAML::Value << static_cast<int>(component.GetCapsuleControllerClimbingMode());
                break;
            default:
                break;
            }

            out << YAML::Key << "Controller Center" << YAML::Value << component.GetControllerCenter();
            
            out << YAML::Key << "Slope Limit" << YAML::Value << component.GetSlopeLimit();
            out << YAML::Key << "Step Offset" << YAML::Value << component.GetStepOffset();
            out << YAML::Key << "Contact Offset" << YAML::Value << component.GetContactOffset();
            out << YAML::Key << "Minimum Movement Distance" << YAML::Value << component.GetMinimumMovementDistance();
            out << YAML::Key << "Non Walkable Mode" << YAML::Value << static_cast<int>(component.GetNonWalkableMode());
            
            out << YAML::Key << "Static Friction" << YAML::Value << component.GetStaticFriction();
            out << YAML::Key << "Dynamic Friction" << YAML::Value << component.GetDynamicFriction();
            out << YAML::Key << "Restitution" << YAML::Value << component.GetRestitution();
        }

        void DeserializeData(YAML::Node& node, CharacterController& component) override
        {
            if (const auto visualizationEnableNode = node["Is Visualization Enabled"])
            {
                component.EnableVisualization(visualizationEnableNode.as<bool>());
            }
            
            if (const auto controllerTypeNode = node["Controller Type"])
            {
                switch (const auto controllerType = static_cast<Controller::Type>(controllerTypeNode.as<int>()))
                {
                case Controller::Type::BoxController:
                    component.SetControllerType(controllerType);
                    if (const auto boxControllerHalfHeightNode = node["Box Controller Half Height"])
                    {
                        component.SetBoxControllerHalfHeight(boxControllerHalfHeightNode.as<float>());
                    }
                    if (const auto boxControllerHalfSideExtent = node["Box Controller Half Side Extent"])
                    {
                        component.SetBoxControllerHalfSideExtent(boxControllerHalfSideExtent.as<float>());
                    }
                    if (const auto boxControllerHalfForwardExtent = node["Box Controller Half Forward Extent"])
                    {
                        component.SetBoxControllerHalfForwardExtent(boxControllerHalfForwardExtent.as<float>());
                    }
                    break;
                case Controller::Type::CapsuleController:
                    component.SetControllerType(controllerType);
                    if (const auto capsuleControllerRadiusNode = node["Capsule Controller Radius"])
                    {
                        component.SetCapsuleControllerRadius(capsuleControllerRadiusNode.as<float>());
                    }
                    if (const auto capsuleControllerHeightNode = node["Capsule Controller Half Height"])
                    {
                        component.SetCapsuleControllerHalfHeight(capsuleControllerHeightNode.as<float>());
                    }
                    if (const auto capsuleControllerClimbingModeNode = node["Capsule Controller Climbing Mode"])
                    {
                        const auto climbingMode = static_cast<CapsuleController::ClimbingMode>(capsuleControllerClimbingModeNode.as<int>());
                        component.SetCapsuleControllerClimbingMode(climbingMode);
                    }
                    break;
                default:
                    break;
                }
            }

            if (const auto controllerCenterNode = node["Controller Center"])
            {
                component.SetControllerCenter(controllerCenterNode.as<Vector3>());
            }

            if (const auto slopeLimitNode = node["Slope Limit"])
            {
                component.SetSlopeLimit(slopeLimitNode.as<float>());
            }
            if (const auto stepOffsetNode = node["Step Offset"])
            {
                component.SetStepOffset(stepOffsetNode.as<float>());
            }
            if (const auto contactOffsetNode = node["Contact Offset"])
            {
                component.SetContactOffset(contactOffsetNode.as<float>());
            }
            if (const auto minimumMovementDistanceNode = node["Minimum Movement Distance"])
            {
                component.SetMinimumMovementDistance(minimumMovementDistanceNode.as<float>());
            }
            if (const auto nonWalkableModeNode = node["Non Walkable Mode"])
            {
                const auto nonWalkableMode = static_cast<Controller::NonWalkableMode>(nonWalkableModeNode.as<int>());
                component.SetNonWalkableMode(nonWalkableMode);
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