#include "pch.h"
#include "ComponentDrawer.h"
#include "Physics/Components/CharacterController.h"

namespace LevEngine::Editor
{
    class CharacterControllerDrawer final : public ComponentDrawer<CharacterController, CharacterControllerDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Physics/Character Controller"; }
        
        void DrawContent(CharacterController& component) override
        {
            EditorGUI::DrawCheckBox("Debug Draw", BindGetter(&CharacterController::IsVisualizationEnabled, &component), BindSetter(&CharacterController::EnableVisualization, &component));
            
            const Array<String, 2> controllerTypeStrings{"Box Controller","Capsule Controller"};
            EditorGUI::DrawComboBox<Controller::Type, 2>("Controller Type", controllerTypeStrings,
                BindGetter(&CharacterController::GetControllerType, &component), BindSetter(&CharacterController::SetControllerType, &component));
            EditorGUI::DrawVector3Control("Center", BindGetter(&CharacterController::GetControllerCenter, &component), BindSetter(&CharacterController::SetControllerCenter, &component));
            if (component.GetControllerType() == Controller::Type::BoxController)
            {
                EditorGUI::DrawFloatControl("Half Height", BindGetter(&CharacterController::GetBoxControllerHalfHeight, &component), BindSetter(&CharacterController::SetBoxControllerHalfHeight, &component), 0.1f, FLT_EPSILON, FLT_MAX);
                EditorGUI::DrawFloatControl("Half Side Extent", BindGetter(&CharacterController::GetBoxControllerHalfSideExtent, &component), BindSetter(&CharacterController::SetBoxControllerHalfSideExtent, &component), 0.1f, FLT_EPSILON, FLT_MAX);
                EditorGUI::DrawFloatControl("Half Forward Extent", BindGetter(&CharacterController::GetBoxControllerHalfForwardExtent, &component), BindSetter(&CharacterController::SetBoxControllerHalfForwardExtent, &component), 0.1f, FLT_EPSILON, FLT_MAX);
            }
            if (component.GetControllerType() == Controller::Type::CapsuleController)
            {
                EditorGUI::DrawFloatControl("Radius", BindGetter(&CharacterController::GetCapsuleControllerRadius, &component), BindSetter(&CharacterController::SetCapsuleControllerRadius, &component), 0.1f, FLT_EPSILON, FLT_MAX);
                EditorGUI::DrawFloatControl("Half Height", BindGetter(&CharacterController::GetCapsuleControllerHalfHeight, &component), BindSetter(&CharacterController::SetCapsuleControllerHalfHeight, &component), 0.1f, FLT_EPSILON, FLT_MAX);
                const Array<String, 2> climbingModeStrings{"Easy","Constrained"};
                EditorGUI::DrawComboBox<CapsuleController::ClimbingMode, 2>("Climbing Mode", climbingModeStrings,
                    BindGetter(&CharacterController::GetCapsuleControllerClimbingMode, &component), BindSetter(&CharacterController::SetCapsuleControllerClimbingMode, &component));
            }

            EditorGUI::DrawFloatControl("Slope Limit", BindGetter(&CharacterController::GetSlopeLimit, &component), BindSetter(&CharacterController::SetSlopeLimit, &component));
            EditorGUI::DrawFloatControl("Step Offset", BindGetter(&CharacterController::GetStepOffset, &component), BindSetter(&CharacterController::SetStepOffset, &component));
            EditorGUI::DrawFloatControl("Contact Offset", BindGetter(&CharacterController::GetContactOffset, &component), BindSetter(&CharacterController::SetContactOffset, &component));
            EditorGUI::DrawFloatControl("Minimum Movement Distance", BindGetter(&CharacterController::GetMinimumMovementDistance, &component), BindSetter(&CharacterController::SetMinimumMovementDistance, &component));
            const Array<String, 2> nonWalkableModeStrings{"Prevent Climbing","Prevent Climbing And Force Sliding"};
            EditorGUI::DrawComboBox<Controller::NonWalkableMode, 2>("Non Walkable Mode", nonWalkableModeStrings,
                BindGetter(&CharacterController::GetNonWalkableMode, &component), BindSetter(&CharacterController::SetNonWalkableMode, &component));
            
            EditorGUI::DrawFloatControl("Static Friction", BindGetter(&CharacterController::GetStaticFriction, &component), BindSetter(&CharacterController::SetStaticFriction, &component), 0.1f, 0.0f, FLT_MAX);
            EditorGUI::DrawFloatControl("Dynamic Friction", BindGetter(&CharacterController::GetDynamicFriction, &component), BindSetter(&CharacterController::SetDynamicFriction, &component), 0.1f, 0.0f, FLT_MAX);
            EditorGUI::DrawFloatControl("Restitution", BindGetter(&CharacterController::GetRestitution, &component), BindSetter(&CharacterController::SetRestitution, &component), 0.1f, 0.0f, FLT_MAX);
        }
    };
}