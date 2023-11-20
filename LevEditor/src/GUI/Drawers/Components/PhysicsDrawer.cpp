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
                for (auto& rigidbodyTypeString : rigidbodyTypeStrings)
                {
                    const bool isSelected = (currentRigidbodyTypeString == rigidbodyTypeString);
                    if (ImGui::Selectable(rigidbodyTypeString, isSelected))
                    {
                        currentRigidbodyTypeString = rigidbodyTypeString;
                        if (currentRigidbodyTypeString == "STATIC")
                        {
                            component.SetRigidbodyType(RigidbodyType::Static);
                            component.SetColliderType(ColliderType::Box);
                        }
                        if (currentRigidbodyTypeString == "DYNAMIC")
                        {
                            component.SetRigidbodyType(RigidbodyType::Dynamic);
                            component.SetColliderType(ColliderType::Box);
                        }
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
            
            if (component.GetColliderNumber() > 0)
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
                                component.SetColliderType(ColliderType::Sphere);
                            }
                            if (currentColliderTypeString == "CAPSULE")
                            {
                                component.SetColliderType(ColliderType::Capsule);
                            }
                            if (currentColliderTypeString == "BOX")
                            {
                                component.SetColliderType(ColliderType::Box);
                            }
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                EditorGUI::DrawVector3Control("Offset Position", BindGetter(&PhysicsRigidbody::GetColliderOffsetPosition, &component), BindSetter(&PhysicsRigidbody::SetColliderOffsetPosition, &component));
                EditorGUI::DrawVector3Control("Offset Rotation", BindGetter(&PhysicsRigidbody::GetColliderOffsetRotation, &component), BindSetter(&PhysicsRigidbody::SetColliderOffsetRotation, &component));
                
                if (currentColliderTypeString == "SPHERE")
                {
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&PhysicsRigidbody::GetSphereRadius, &component), BindSetter(&PhysicsRigidbody::SetSphereRadius, &component), 1.0f, 0.0f, FLT_MAX);
                }
                if (currentColliderTypeString == "CAPSULE")
                {
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&PhysicsRigidbody::GetCapsuleRadius, &component), BindSetter(&PhysicsRigidbody::SetCapsuleRadius, &component), 1.0f, 0.0f, FLT_MAX);
                    EditorGUI::DrawFloatControl("Half Height", BindGetter(&PhysicsRigidbody::GetCapsuleHalfHeight, &component), BindSetter(&PhysicsRigidbody::SetCapsuleHalfHeight, &component), 1.0f, 0.0f, FLT_MAX);
                }
                if (currentColliderTypeString == "BOX")
                {
                    EditorGUI::DrawVector3Control("Half Extends", BindGetter(&PhysicsRigidbody::GetBoxHalfExtents, &component), BindSetter(&PhysicsRigidbody::SetBoxHalfExtents, &component),1.0f, 0.0f, FLT_MAX);
                }
                
                EditorGUI::DrawFloatControl("Static Friction", BindGetter(&PhysicsRigidbody::GetStaticFriction, &component), BindSetter(&PhysicsRigidbody::SetStaticFriction, &component), 1.0f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Dynamic Friction", BindGetter(&PhysicsRigidbody::GetDynamicFriction, &component), BindSetter(&PhysicsRigidbody::SetDynamicFriction, &component), 1.0f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Restitution", BindGetter(&PhysicsRigidbody::GetRestitution, &component), BindSetter(&PhysicsRigidbody::SetRestitution, &component), 1.0f, 0.0f, 1.0f);
            }
        }
    };
}
