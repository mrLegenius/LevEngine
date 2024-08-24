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
            if (ImGui::TreeNode("Layer"))
            {
                const Array<String, 10> layerStrings
                {
                    "Layer0", "Layer1", "Layer2", "Layer3", "Layer4",
                    "Layer5", "Layer6", "Layer7", "Layer8", "Layer9"
                };
                EditorGUI::DrawFlagComboBox<FilterLayer, 10>(
                    "Layer",
                    layerStrings,
                    BindGetter(&CharacterController::GetLayer, &component),
                    BindSetter(&CharacterController::SetLayer, &component)
                );
                ImGui::TreePop();
            }
            
            EditorGUI::DrawFloatControl("Slope Limit", BindGetter(&CharacterController::GetSlopeLimit, &component), BindSetter(&CharacterController::SetSlopeLimit, &component), 1.0f, 0.0f, 45.0f);
            EditorGUI::DrawFloatControl("Step Offset", BindGetter(&CharacterController::GetStepOffset, &component), BindSetter(&CharacterController::SetStepOffset, &component), 0.1f, 0.0f, FLT_MAX);
            EditorGUI::DrawFloatControl("Skin Width", BindGetter(&CharacterController::GetSkinWidth, &component), BindSetter(&CharacterController::SetSkinWidth, &component), 0.1f, FLT_EPSILON, FLT_MAX);
            EditorGUI::DrawFloatControl("Min Move Distance", BindGetter(&CharacterController::GetMinMoveDistance, &component), BindSetter(&CharacterController::SetMinMoveDistance, &component), 0.1f, 0.0f, FLT_MAX);
            
            EditorGUI::DrawVector3Control("Center", BindGetter(&CharacterController::GetCenterOffset, &component), BindSetter(&CharacterController::SetCenterOffset, &component));
            EditorGUI::DrawFloatControl("Radius", BindGetter(&CharacterController::GetRadius, &component), BindSetter(&CharacterController::SetRadius, &component), 0.1f, FLT_EPSILON, FLT_MAX);
            EditorGUI::DrawFloatControl("Height", BindGetter(&CharacterController::GetHeight, &component), BindSetter(&CharacterController::SetHeight, &component), 0.1f, 0.0f, FLT_MAX);

            if (ImGui::TreeNode("Physic Material"))
            {
                EditorGUI::DrawFloatControl("Dynamic Friction", BindGetter(&CharacterController::GetDynamicFriction, &component), BindSetter(&CharacterController::SetDynamicFriction, &component), 0.1f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Static Friction", BindGetter(&CharacterController::GetStaticFriction, &component), BindSetter(&CharacterController::SetStaticFriction, &component), 0.1f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Bounciness", BindGetter(&CharacterController::GetBounciness, &component), BindSetter(&CharacterController::SetBounciness, &component), 0.1f, 0.0f, 1.0f);
                const Array<String, 4> frictionCombineModeStrings {"Average","Minimum", "Multiply", "Maximum"};
                EditorGUI::DrawComboBox<PhysicMaterial::CombineMode, 4>("Friction Combine", frictionCombineModeStrings,
                    BindGetter(&CharacterController::GetFrictionCombineMode, &component), BindSetter(&CharacterController::SetFrictionCombineMode, &component));
                const Array<String, 4> bounceCombineModeStrings {"Average","Minimum", "Multiply", "Maximum"};
                EditorGUI::DrawComboBox<PhysicMaterial::CombineMode, 4>("Bounce Combine", frictionCombineModeStrings,
                    BindGetter(&CharacterController::GetBounceCombineMode, &component), BindSetter(&CharacterController::SetBounceCombineMode, &component));
               ImGui::TreePop(); 
            }
        }
    };
}