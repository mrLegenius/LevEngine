#include "pch.h"
#include "ComponentDrawer.h"

namespace LevEngine::Editor
{
    class RigidbodyDrawer final : public ComponentDrawer<Rigidbody, RigidbodyDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Physics/Rigidbody"; }
        
        void DrawContent(Rigidbody& component) override
        {
            if (ImGui::TreeNode("Layer"))
            {
                const Array<String, 10> layerStrings
                {
                    "Layer0", "Layer1", "Layer2", "Layer3", "Layer4",
                    "Layer5", "Layer6", "Layer7", "Layer8", "Layer9"
                };
                EditorGUI::DrawComboBox<FilterLayer, 10>("Layer", layerStrings,
                    BindGetter(&Rigidbody::GetLayer, &component), BindSetter(&Rigidbody::SetLayer, &component));
                ImGui::TreePop();
            }
            
            if (!component.IsKinematicEnabled())
            {
                const Array<String, 2> rigidbodyTypeStrings {"Static","Dynamic"};
                EditorGUI::DrawComboBox<Rigidbody::Type, 2>("Rigidbody Type", rigidbodyTypeStrings,
                    BindGetter(&Rigidbody::GetRigidbodyType, &component), BindSetter(&Rigidbody::SetRigidbodyType, &component));
            }

            if (component.GetRigidbodyType() == Rigidbody::Type::Dynamic)
            {
                if (!component.IsKinematicEnabled())
                {
                    EditorGUI::DrawFloatControl("Mass", BindGetter(&Rigidbody::GetMass, &component), BindSetter(&Rigidbody::SetMass, &component));

                    EditorGUI::DrawFloatControl("Linear Damping", BindGetter(&Rigidbody::GetLinearDamping, &component), BindSetter(&Rigidbody::SetLinearDamping, &component));
                    EditorGUI::DrawFloatControl("Angular Damping", BindGetter(&Rigidbody::GetAngularDamping, &component), BindSetter(&Rigidbody::SetAngularDamping, &component));

                    EditorGUI::DrawVector3Control("Center Of Mass", BindGetter(&Rigidbody::GetCenterOfMass, &component), BindSetter(&Rigidbody::SetCenterOfMass, &component));
                    EditorGUI::DrawVector3Control("Diagonal Inertia", BindGetter(&Rigidbody::GetInertiaTensor, &component), BindSetter(&Rigidbody::SetInertiaTensor, &component));

                    EditorGUI::DrawCheckBox("Enable Gravity", BindGetter(&Rigidbody::IsGravityEnabled, &component), BindSetter(&Rigidbody::EnableGravity, &component));
                }

                EditorGUI::DrawCheckBox("Enable Kinematic", BindGetter(&Rigidbody::IsKinematicEnabled, &component), BindSetter(&Rigidbody::EnableKinematic, &component));

                if (!component.IsKinematicEnabled())
                {
                    if (ImGui::TreeNode("Constraints"))
                    {
                        EditorGUI::DrawCheckBox3Control(
                            "Position",
                            "X", BindGetter(&Rigidbody::IsPosAxisXLocked, &component), BindSetter(&Rigidbody::LockPosAxisX, &component),
                            "Y", BindGetter(&Rigidbody::IsPosAxisYLocked, &component), BindSetter(&Rigidbody::LockPosAxisY, &component),
                            "Z", BindGetter(&Rigidbody::IsPosAxisZLocked, &component), BindSetter(&Rigidbody::LockPosAxisZ, &component)
                        );
                        EditorGUI::DrawCheckBox3Control(
                            "Rotation",
                            "X", BindGetter(&Rigidbody::IsRotAxisXLocked, &component), BindSetter(&Rigidbody::LockRotAxisX, &component),
                            "Y", BindGetter(&Rigidbody::IsRotAxisYLocked, &component), BindSetter(&Rigidbody::LockRotAxisY, &component),
                            "Z", BindGetter(&Rigidbody::IsRotAxisZLocked, &component), BindSetter(&Rigidbody::LockRotAxisZ, &component)
                        );
                        ImGui::TreePop();
                    }
                }
            }

            const Array<String, 3> colliderTypeStrings{"Sphere","Capsule", "Box"};
            EditorGUI::DrawComboBox<Collider::Type, 3>("Collider Type", colliderTypeStrings,
                BindGetter(&Rigidbody::GetColliderType, &component), BindSetter(&Rigidbody::SetColliderType, &component));

            EditorGUI::DrawCheckBox("Enable Trigger", BindGetter(&Rigidbody::IsTriggerEnabled, &component), BindSetter(&Rigidbody::EnableTrigger, &component));
            
            EditorGUI::DrawVector3Control("Offset Position", BindGetter(&Rigidbody::GetColliderOffsetPosition, &component), BindSetter(&Rigidbody::SetColliderOffsetPosition, &component));
            EditorGUI::DrawVector3Control("Offset Rotation", BindGetter(&Rigidbody::GetColliderOffsetRotation, &component), BindSetter(&Rigidbody::SetColliderOffsetRotation, &component));
            
            if (component.GetColliderType() == Collider::Type::Sphere)
            {
                EditorGUI::DrawFloatControl("Radius", BindGetter(&Rigidbody::GetSphereRadius, &component), BindSetter(&Rigidbody::SetSphereRadius, &component), 0.1f, FLT_EPSILON, FLT_MAX);
            }
            if (component.GetColliderType() == Collider::Type::Capsule)
            {
                EditorGUI::DrawFloatControl("Radius", BindGetter(&Rigidbody::GetCapsuleRadius, &component), BindSetter(&Rigidbody::SetCapsuleRadius, &component), 0.1f, FLT_EPSILON, FLT_MAX);
                EditorGUI::DrawFloatControl("Half Height", BindGetter(&Rigidbody::GetCapsuleHalfHeight, &component), BindSetter(&Rigidbody::SetCapsuleHalfHeight, &component), 0.1f, FLT_EPSILON, FLT_MAX);
            }
            if (component.GetColliderType() == Collider::Type::Box)
            {
                EditorGUI::DrawVector3Control("Half Extends", BindGetter(&Rigidbody::GetBoxHalfExtents, &component), BindSetter(&Rigidbody::SetBoxHalfExtents, &component), 0.1f, FLT_EPSILON, FLT_MAX);
            }
            
            if (ImGui::TreeNode("Physic Material"))
            {
                EditorGUI::DrawFloatControl("Static Friction", BindGetter(&Rigidbody::GetStaticFriction, &component), BindSetter(&Rigidbody::SetStaticFriction, &component), 0.1f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Dynamic Friction", BindGetter(&Rigidbody::GetDynamicFriction, &component), BindSetter(&Rigidbody::SetDynamicFriction, &component), 0.1f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Bounciness", BindGetter(&Rigidbody::GetBounciness, &component), BindSetter(&Rigidbody::SetBounciness, &component), 0.1f, 0.0f, FLT_MAX);
                const Array<String, 4> frictionCombineModeStrings {"Average","Minimum", "Multiply", "Maximum"};
                EditorGUI::DrawComboBox<PhysicMaterial::CombineMode, 4>("Friction Combine", frictionCombineModeStrings,
                    BindGetter(&Rigidbody::GetFrictionCombineMode, &component), BindSetter(&Rigidbody::SetFrictionCombineMode, &component));
                const Array<String, 4> bounceCombineModeStrings {"Average","Minimum", "Multiply", "Maximum"};
                EditorGUI::DrawComboBox<PhysicMaterial::CombineMode, 4>("Bounce Combine", frictionCombineModeStrings,
                    BindGetter(&Rigidbody::GetBounceCombineMode, &component), BindSetter(&Rigidbody::SetBounceCombineMode, &component));
                ImGui::TreePop();
            }
        }
    };
}
