#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"
#include "Physics/PhysX/PhysicsRigidbody.h"

namespace LevEngine::Editor
{
    class PhysicsDrawer final : public ComponentDrawer<PhysicsRigidbody, PhysicsDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Physics"; }
        
        void DrawContent(PhysicsRigidbody& component) override
        {
            EditorGUI::DrawCheckBox("Draw Collider", BindGetter(&PhysicsRigidbody::GetColliderVisualizationFlag, &component), BindSetter(&PhysicsRigidbody::SetColliderVisualizationFlag, &component));
            
            const char* rigidbodyTypeStrings[] = { "STATIC", "DYNAMIC" };
            const char* currentRigidbodyTypeString = rigidbodyTypeStrings[static_cast<int>(component.GetRigidbodyType())];
            if (ImGui::BeginCombo("Rigidbody Type", currentRigidbodyTypeString))
            {
                for (int i = 0; i < 2; i++)
                {
                    const bool isSelected = (currentRigidbodyTypeString == rigidbodyTypeStrings[i]);
                    if (ImGui::Selectable(rigidbodyTypeStrings[i], isSelected))
                    {
                        currentRigidbodyTypeString = rigidbodyTypeStrings[i];
                        component.SetRigidbodyType(static_cast<RigidbodyType>(i));
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (currentRigidbodyTypeString == "DYNAMIC")
            {
                EditorGUI::DrawCheckBox("Enable Gravity", BindGetter(&PhysicsRigidbody::GetRigidbodyGravityFlag, &component), BindSetter(&PhysicsRigidbody::SetRigidbodyGravityFlag, &component));
            }
            
            if (component.GetRigidbody()->getNbShapes() > 0)
            {
                const char* colliderTypeStrings[] = { "SPHERE", "CAPSULE", "BOX" };
                const char* currentColliderTypeString = colliderTypeStrings[static_cast<int>(component.GetColliderType())];
                if (ImGui::BeginCombo("Collider Type", currentColliderTypeString))
                {
                    for (auto& colliderTypeString : colliderTypeStrings)
                    {
                        const bool isSelected = (currentColliderTypeString == colliderTypeString);
                        if (ImGui::Selectable(colliderTypeString, isSelected))
                        {
                            currentColliderTypeString = colliderTypeString;
                            if (currentColliderTypeString == "SPHERE")
                            {
                                component.AttachCollider(ColliderType::Sphere);
                            }
                            if (currentColliderTypeString == "CAPSULE")
                            {
                                component.AttachCollider(ColliderType::Capsule);
                            }
                            if (currentColliderTypeString == "BOX")
                            {
                                component.AttachCollider(ColliderType::Box);
                            }
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                EditorGUI::DrawVector3Control("Offset Position", BindGetter(&PhysicsRigidbody::GetShapeLocalPosition, &component), BindSetter(&PhysicsRigidbody::SetShapeLocalPosition, &component), 1.0f, 0.0f, FLT_MAX);
                EditorGUI::DrawVector3Control("Offset Rotation", BindGetter(&PhysicsRigidbody::GetShapeLocalRotation, &component), BindSetter(&PhysicsRigidbody::SetShapeLocalRotation, &component), 1.0f, 0.0f, FLT_MAX);
                
                if (currentColliderTypeString == "SPHERE")
                {
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&PhysicsRigidbody::GetSphereColliderRadius, &component), BindSetter(&PhysicsRigidbody::SetSphereColliderRadius, &component), 1.0f, 0.0f, FLT_MAX);
                }
                if (currentColliderTypeString == "CAPSULE")
                {
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&PhysicsRigidbody::GetCapsuleColliderRadius, &component), BindSetter(&PhysicsRigidbody::SetCapsuleColliderRadius, &component), 1.0f, 0.0f, FLT_MAX);
                    EditorGUI::DrawFloatControl("Half Height", BindGetter(&PhysicsRigidbody::GetCapsuleColliderHalfHeight, &component), BindSetter(&PhysicsRigidbody::SetCapsuleColliderHalfHeight, &component), 1.0f, 0.0f, FLT_MAX);
                }
                if (currentColliderTypeString == "BOX")
                {
                    EditorGUI::DrawVector3Control("Half Extends", BindGetter(&PhysicsRigidbody::GetBoxHalfExtends, &component), BindSetter(&PhysicsRigidbody::SetBoxHalfExtends, &component),1.0f, 0.0f, FLT_MAX);
                }
                
                EditorGUI::DrawFloatControl("Static Friction", BindGetter(&PhysicsRigidbody::GetStaticFriction, &component), BindSetter(&PhysicsRigidbody::SetStaticFriction, &component), 1.0f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Dynamic Friction", BindGetter(&PhysicsRigidbody::GetDynamicFriction, &component), BindSetter(&PhysicsRigidbody::SetDynamicFriction, &component), 1.0f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Restitution", BindGetter(&PhysicsRigidbody::GetRestitution, &component), BindSetter(&PhysicsRigidbody::SetRestitution, &component), 1.0f, 0.0f, 1.0f);
            }
        }
    };
}
