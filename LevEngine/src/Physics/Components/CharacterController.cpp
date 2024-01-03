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
            SetCapsuleControllerRadius(GetCapsuleControllerRadius());
            SetCapsuleControllerHalfHeight(GetCapsuleControllerHalfHeight());
            SetCapsuleControllerClimbingMode(GetCapsuleControllerClimbingMode());
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

    float CharacterController::GetCapsuleControllerRadius() const
    {
        return m_CharacterController->Radius;
    }

    void CharacterController::SetCapsuleControllerRadius(const float radius) const
    {
        if (radius <= 0.0f) return;
        
        m_CharacterController->Radius = radius;

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
        return m_CharacterController->HalfHeight;
    }

    void CharacterController::SetCapsuleControllerHalfHeight(const float halfHeight) const
    {
        if (halfHeight <= 0.0f) return;
        
        m_CharacterController->HalfHeight = halfHeight;

        if (m_Controller != NULL)
        {
            const float maxTransformScale = Math::MaxElement(m_TransformScale);
            
            const auto collider = GetCollider();
            const physx::PxGeometryHolder geom(collider->getGeometry());
            const auto initialRadius = geom.capsule().radius;
            collider->setGeometry(physx::PxCapsuleGeometry(initialRadius, halfHeight * maxTransformScale));
        }
    }

    Controller::ClimbingMode CharacterController::GetCapsuleControllerClimbingMode() const
    {
        return m_CharacterController->m_ClimbingMode;
    }

    void CharacterController::SetCapsuleControllerClimbingMode(const Controller::ClimbingMode& climbingMode) const
    {
        m_CharacterController->m_ClimbingMode = climbingMode;

        if (m_Controller != NULL)
        {
            switch (climbingMode)
            {
            case Controller::ClimbingMode::Easy:
                static_cast<physx::PxCapsuleController*>(m_Controller)->setClimbingMode(physx::PxCapsuleClimbingMode::eEASY);
                break;
            case Controller::ClimbingMode::Constrained:
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

    void CharacterController::Move(const Vector3 displacement, const float elapsedTime) const
    {
        if (m_Controller != NULL)
        {
            m_Controller->move(
                PhysicsUtils::FromVector3ToPxVec3(displacement),
                GetMinimumMovementDistance(),
                elapsedTime,
                physx::PxControllerFilters()
            );
        }
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

        m_Controller =
            App::Get().GetPhysics().CreateCapsuleController(
                entity,
                GetCapsuleControllerRadius(),
                GetCapsuleControllerHalfHeight(),
                GetCapsuleControllerClimbingMode(),
                physicalMaterial
            );

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
            
            out << YAML::Key << "Capsule Controller Radius" << YAML::Value << component.GetCapsuleControllerRadius();
            out << YAML::Key << "Capsule Controller Half Height" << YAML::Value << component.GetCapsuleControllerHalfHeight();
            out << YAML::Key << "Capsule Controller Climbing Mode" << YAML::Value << static_cast<int>(component.GetCapsuleControllerClimbingMode());

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
                const auto climbingMode = static_cast<Controller::ClimbingMode>(capsuleControllerClimbingModeNode.as<int>());
                component.SetCapsuleControllerClimbingMode(climbingMode);
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