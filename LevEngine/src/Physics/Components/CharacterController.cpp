﻿#include "levpch.h"
#include "CharacterController.h"

#include "Kernel/Application.h"
#include "Kernel/Time/Timestep.h"
#include "Physics/PhysicsUtils.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsSettings.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    void CharacterController::OnConstruct(const Entity entity)
    {
        auto& controller = entity.GetComponent<CharacterController>();
        controller.Initialize(entity);
    }

    void CharacterController::OnDestroy(const Entity entity)
    {
        auto& controller = entity.GetComponent<CharacterController>();

        if (controller.m_Controller == nullptr) return;

        controller.DetachController();
    }

    void CharacterController::Initialize(const Entity entity)
    {
        AttachController(entity);
        
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
        const auto actor = GetActor();
        const auto nbColliders = actor->getNbShapes();
        actor->getShapes(collider, nbColliders);

        return *collider;
    }

    physx::PxMaterial* CharacterController::GetPhysicalMaterial() const
    {
        physx::PxMaterial* physicalMaterial[1];
        const auto collider = GetCollider();
        const auto nbMaterials = collider->getNbMaterials();
        collider->getMaterials(physicalMaterial, nbMaterials);

        return *physicalMaterial;
    }
    
    Vector3 CharacterController::GetTransformScale() const
    {
        return m_TransformScale;
    }

    void CharacterController::SetTransformScale(const Vector3 scale)
    {
        m_TransformScale = scale;
        
        SetRadius(GetRadius());
        SetHeight(GetHeight());
    }

    bool CharacterController::IsVisualizationEnabled() const
    {
        return m_IsVisualizationEnabled;
    }
    
    void CharacterController::EnableVisualization(const bool flag)
    {
        m_IsVisualizationEnabled = flag;
        
        if (m_Controller != nullptr)
        {
            GetActor()->setActorFlag(physx::PxActorFlag::eVISUALIZATION, flag);
        }
    }

    void CharacterController::AttachController(const Entity entity)
    {
        if (m_Controller != nullptr)
        {
            DetachController();
        }

        m_Controller =
            App::Get().GetPhysics().CreateCapsuleController(
                entity,
                GetRadius(),
                GetHeight()
            );

        EnableVisualization(IsVisualizationEnabled());

        SetLayer(GetLayer());

        SetSlopeLimit(GetSlopeLimit());
        SetStepOffset(GetStepOffset());
        SetSkinWidth(GetSkinWidth());
        SetMinMoveDistance(GetMinMoveDistance());
        SetCenterOffset(GetCenterOffset());
        SetRadius(GetRadius());
        SetHeight(GetHeight());
        SetClimbingMode(GetClimbingMode());
        SetNonWalkableMode(GetNonWalkableMode());
        SetStaticFriction(GetStaticFriction());
        SetDynamicFriction(GetDynamicFriction());
        SetBounciness(GetBounciness());
        SetFrictionCombineMode(GetFrictionCombineMode());
        SetBounceCombineMode(GetBounceCombineMode());
    }

    void CharacterController::DetachController()
    {
        App::Get().GetPhysics().RemoveController(m_Controller);
        m_Controller = nullptr;
    }
    
    FilterLayer CharacterController::GetLayer() const
    {
        return m_CharacterController->m_Layer;
    }

    void CharacterController::SetLayer(const FilterLayer& layer) const
    {
        m_CharacterController->m_Layer = layer;

        if (m_Controller != nullptr)
        {
            physx::PxFilterData filterData;
            filterData.word0 = static_cast<physx::PxU32>(layer);
            const auto collisions = PhysicsSettings::GetLayerCollisions(layer);
            filterData.word1 = static_cast<physx::PxU32>(collisions);
            GetCollider()->setSimulationFilterData(filterData);
            GetCollider()->setQueryFilterData(filterData);
        }
    }

    float CharacterController::GetSlopeLimit() const
    {
        return m_CharacterController->SlopeLimit;
    }

    void CharacterController::SetSlopeLimit(const float slopeLimit) const
    {
        if (slopeLimit < 0.0f || slopeLimit > 45.0f) return;

        m_CharacterController->SlopeLimit = slopeLimit;

        if (m_Controller != nullptr)
        {
            auto cosine = 0.0f;
            if (Math::IsEqual(slopeLimit, 45.0f))
            {
                cosine = std::cosf(slopeLimit * Math::DegToRad);
            }
            m_Controller->setSlopeLimit(cosine);
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

        if (m_Controller != nullptr)
        {
            m_Controller->setStepOffset(stepOffset);
        }
    }

    float CharacterController::GetSkinWidth() const
    {
        return m_CharacterController->SkinWidth;
    }

    void CharacterController::SetSkinWidth(const float contactOffset) const
    {
        if (contactOffset <= 0.0f) return;
        
        m_CharacterController->SkinWidth = contactOffset;

        if (m_Controller != nullptr)
        {
            m_Controller->setContactOffset(contactOffset);
        }
    }

    float CharacterController::GetMinMoveDistance() const
    {
        return m_CharacterController->MinMoveDistance;
    }

    void CharacterController::SetMinMoveDistance(const float minimumMovementDistance) const
    {
        if (minimumMovementDistance < 0.0f) return;
        
        m_CharacterController->MinMoveDistance = minimumMovementDistance;
    }

    Vector3 CharacterController::GetCenterOffset() const
    {
        return m_CharacterController->CenterOffset;
    }

    void CharacterController::SetCenterOffset(const Vector3 center) const
    {
        if (m_Controller != nullptr)
        {
            const auto transformPosition =
                PhysicsUtils::FromPxExtendedVec3ToVector3(m_Controller->getPosition()) - GetCenterOffset();
            m_Controller->setPosition(PhysicsUtils::FromVector3ToPxExtendedVec3(transformPosition + center));
        }

        m_CharacterController->CenterOffset = center;
    }

    float CharacterController::GetRadius() const
    {
        return m_CharacterController->Radius;
    }

    void CharacterController::SetRadius(const float radius) const
    {
        if (radius <= 0.0f) return;

        m_CharacterController->Radius = radius;

        if (m_Controller != nullptr)
        {
            static_cast<physx::PxCapsuleController*>(m_Controller)->setRadius(radius);
        }
    }

    float CharacterController::GetHeight() const
    {
        return m_CharacterController->Height;
    }

    void CharacterController::SetHeight(const float height) const
    {
        if (height < 0.0f) return;

        m_CharacterController->Height = height;

        if (m_Controller != nullptr)
        {
            static_cast<physx::PxCapsuleController*>(m_Controller)->setHeight(height);
        }
    }

    float CharacterController::GetDynamicFriction() const
    {
        return m_CharacterController->m_PhysicalMaterial->DynamicFriction;
    }

    void CharacterController::SetDynamicFriction(const float dynamicFriction) const
    {
        if (dynamicFriction < 0.0f) return;
        
        m_CharacterController->m_PhysicalMaterial->DynamicFriction = dynamicFriction;
        
        if (m_Controller != nullptr)
        {
            const auto physicalMaterials = GetPhysicalMaterial();
            physicalMaterials[0].setDynamicFriction(dynamicFriction);
        }
    }
    
    float CharacterController::GetStaticFriction() const
    {
        return m_CharacterController->m_PhysicalMaterial->StaticFriction;
    }

    void CharacterController::SetStaticFriction(const float staticFriction) const
    {
        if (staticFriction < 0.0f) return;
        
        m_CharacterController->m_PhysicalMaterial->StaticFriction = staticFriction;
        
        if (m_Controller != nullptr)
        {
            const auto physicalMaterials = GetPhysicalMaterial();
            physicalMaterials[0].setStaticFriction(staticFriction);
        }
    }
    
    float CharacterController::GetBounciness() const
    {
        return m_CharacterController->m_PhysicalMaterial->Bounciness;
    }

    void CharacterController::SetBounciness(const float bounciness) const
    {
        if (bounciness < 0.0f || bounciness > 1.0f) return;
        
        m_CharacterController->m_PhysicalMaterial->Bounciness = bounciness;
        
        if (m_Controller != nullptr)
        {
            const auto physicalMaterials = GetPhysicalMaterial();
            physicalMaterials[0].setRestitution(bounciness);
        }
    }

    PhysicMaterial::CombineMode CharacterController::GetFrictionCombineMode() const
    {
        return m_CharacterController->m_PhysicalMaterial->m_FrictionCombine;
    }

    void CharacterController::SetFrictionCombineMode(const PhysicMaterial::CombineMode& frictionCombineMode) const
    {
        m_CharacterController->m_PhysicalMaterial->m_FrictionCombine = frictionCombineMode;

        if (m_Controller != nullptr)
        {
            const auto physicalMaterials = GetPhysicalMaterial();
            physicalMaterials[0].setFrictionCombineMode(
                static_cast<physx::PxCombineMode::Enum>(static_cast<int>(frictionCombineMode))
            );
        }
    }

    PhysicMaterial::CombineMode CharacterController::GetBounceCombineMode() const
    {
        return m_CharacterController->m_PhysicalMaterial->m_BounceCombine;
    }

    void CharacterController::SetBounceCombineMode(const PhysicMaterial::CombineMode& bounceCombineMode) const
    {
        m_CharacterController->m_PhysicalMaterial->m_BounceCombine = bounceCombineMode;

        if (m_Controller != nullptr)
        {
            const auto physicalMaterials = GetPhysicalMaterial();
            physicalMaterials[0].setRestitutionCombineMode(
                static_cast<physx::PxCombineMode::Enum>(static_cast<int>(bounceCombineMode))
            );
        }
    }

    Controller::NonWalkableMode CharacterController::GetNonWalkableMode() const
    {
        return m_CharacterController->m_NonWalkableMode;
    }

    void CharacterController::SetNonWalkableMode(const Controller::NonWalkableMode& nonWalkableMode) const
    {
        m_CharacterController->m_NonWalkableMode = nonWalkableMode;

        if (m_Controller != nullptr)
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

    Controller::ClimbingMode CharacterController::GetClimbingMode() const
    {
        return m_CharacterController->m_ClimbingMode;
    }

    void CharacterController::SetClimbingMode(const Controller::ClimbingMode& climbingMode) const
    {
        m_CharacterController->m_ClimbingMode = climbingMode;

        if (m_Controller != nullptr)
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

    float CharacterController::GetGravityScale() const
    {
        return m_CharacterController->GravityScale;
    }

    void CharacterController::SetGravityScale(const float gravityScale) const
    {
        if (gravityScale < 0.0f) return;

        m_CharacterController->GravityScale = gravityScale;
    }

    bool CharacterController::IsGrounded() const
    {
        return m_CharacterController->IsGrounded;
    }

    void CharacterController::SetGroundFlag(const bool flag) const
    {
        m_CharacterController->IsGrounded = flag;
    }

    float CharacterController::GetVerticalVelocity() const
    {
        return m_CharacterController->VerticalVelocity;
    }

    void CharacterController::SetVerticalVelocity(const float verticalVelocity) const
    {
        m_CharacterController->VerticalVelocity = verticalVelocity;
    }

    void CharacterController::Move(const Vector3 displacement)
    {
        if (m_Controller == nullptr) return;

        const auto currentTimePoint = steady_clock::now();
        const auto currentTime =
            std::chrono::duration_cast<microseconds>(currentTimePoint.time_since_epoch()).count() / 1000000.0f;
        const auto elapsedTime = currentTime - m_LastMoveTime.GetSeconds();
        
        m_Controller->move(
            PhysicsUtils::FromVector3ToPxVec3(displacement),
            GetMinMoveDistance(),
            elapsedTime,
            physx::PxControllerFilters()
        );
        
        m_LastMoveTime = Timestep(currentTime);
    }

    void CharacterController::MoveTo(const Vector3 position)
    {
        if (m_Controller == nullptr) return;

        const Vector3 displacement =
            position - PhysicsUtils::FromPxExtendedVec3ToVector3(m_Controller->getPosition());
        Move(displacement);
    }

    void CharacterController::Teleport(const Vector3 position)
    {
        if (m_Controller == nullptr) return;

        m_Controller->setPosition(PhysicsUtils::FromVector3ToPxExtendedVec3(position));
    }

    void CharacterController::Jump(const float jumpHeight, const float deltaTime)
    {
        if (m_Controller == nullptr) return;

        if (!IsGrounded()) return;

        const auto gravity = App::Get().GetPhysics().GetGravity();
    
        const auto verticalVelocity =
            std::sqrtf(jumpHeight * -2.0f * gravity.y * GetGravityScale());
        SetVerticalVelocity(verticalVelocity);

        const auto jumpDisplacement = verticalVelocity * deltaTime;
        Move({0.0f, jumpDisplacement, 0.0f});
    }

    const Vector<ControllerColliderHit>& CharacterController::GetCollisionHitBuffer() const
    {
        return m_CollisionHitBuffer; 
    }

    class CharacterControllerSerializer final : public ComponentSerializer<CharacterController, CharacterControllerSerializer>
    {
    protected:
        const char* GetKey() override { return "Character Controller"; }

        void SerializeData(YAML::Emitter& out, const CharacterController& component) override
        {
            out << YAML::Key << "Layer" << YAML::Value << static_cast<int>(component.GetLayer());

            out << YAML::Key << "Capsule Controller Radius" << YAML::Value << component.GetRadius();
            out << YAML::Key << "Capsule Controller Half Height" << YAML::Value << component.GetHeight();
            out << YAML::Key << "Capsule Controller Climbing Mode" << YAML::Value << static_cast<int>(component.GetClimbingMode());

            out << YAML::Key << "Controller Center" << YAML::Value << component.GetCenterOffset();
            
            out << YAML::Key << "Slope Limit" << YAML::Value << component.GetSlopeLimit();
            out << YAML::Key << "Step Offset" << YAML::Value << component.GetStepOffset();
            out << YAML::Key << "Contact Offset" << YAML::Value << component.GetSkinWidth();
            out << YAML::Key << "Minimum Movement Distance" << YAML::Value << component.GetMinMoveDistance();
            out << YAML::Key << "Non Walkable Mode" << YAML::Value << static_cast<int>(component.GetNonWalkableMode());

            out << YAML::Key << "Dynamic Friction" << YAML::Value << component.GetDynamicFriction();
            out << YAML::Key << "Static Friction" << YAML::Value << component.GetStaticFriction();
            out << YAML::Key << "Bounciness" << YAML::Value << component.GetBounciness();
            out << YAML::Key << "Friction Combine Mode" << YAML::Value << static_cast<int>(component.GetFrictionCombineMode());
            out << YAML::Key << "Bounce Combine Mode" << YAML::Value << static_cast<int>(component.GetBounceCombineMode());
        }

        void DeserializeData(const YAML::Node& node, CharacterController& component) override
        {

            if (const auto layerNode = node["Layer"])
            {
                const auto layer = static_cast<FilterLayer>(layerNode.as<int>());
                component.SetLayer(layer);
            }
            
            if (const auto capsuleControllerRadiusNode = node["Capsule Controller Radius"])
            {
                component.SetRadius(capsuleControllerRadiusNode.as<float>());
            }
            if (const auto capsuleControllerHeightNode = node["Capsule Controller Half Height"])
            {
                component.SetHeight(capsuleControllerHeightNode.as<float>());
            }
            
            if (const auto capsuleControllerClimbingModeNode = node["Capsule Controller Climbing Mode"])
            {
                const auto climbingMode = static_cast<Controller::ClimbingMode>(capsuleControllerClimbingModeNode.as<int>());
                component.SetClimbingMode(climbingMode);
            }

            if (const auto controllerCenterNode = node["Controller Center"])
            {
                component.SetCenterOffset(controllerCenterNode.as<Vector3>());
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
                component.SetSkinWidth(contactOffsetNode.as<float>());
            }
            
            if (const auto minimumMovementDistanceNode = node["Minimum Movement Distance"])
            {
                component.SetMinMoveDistance(minimumMovementDistanceNode.as<float>());
            }
            
            if (const auto nonWalkableModeNode = node["Non Walkable Mode"])
            {
                const auto nonWalkableMode = static_cast<Controller::NonWalkableMode>(nonWalkableModeNode.as<int>());
                component.SetNonWalkableMode(nonWalkableMode);
            }

            if (const auto dynamicFrictionNode = node["Dynamic Friction"])
            {
                component.SetDynamicFriction(dynamicFrictionNode.as<float>());
            }
            if (const auto staticFrictionNode = node["Static Friction"])
            {
                component.SetStaticFriction(staticFrictionNode.as<float>());
            }
            if (const auto restitutionNode = node["Bounciness"])
            {
                component.SetBounciness(restitutionNode.as<float>());
            }
            if (const auto frictionCombineModeNode = node["Friction Combine Mode"])
            {
                const auto frictionCombineMode = static_cast<PhysicMaterial::CombineMode>(frictionCombineModeNode.as<int>());
                component.SetFrictionCombineMode(frictionCombineMode);
            }
            if (const auto bounceCombineModeNode = node["Bounce Combine Mode"])
            {
                const auto bounceCombineMode = static_cast<PhysicMaterial::CombineMode>(bounceCombineModeNode.as<int>());
                component.SetBounceCombineMode(bounceCombineMode);
            }
        } 
    };
}