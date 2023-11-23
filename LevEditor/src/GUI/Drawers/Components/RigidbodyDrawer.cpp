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
            EditorGUI::DrawCheckBox("Draw Collider", BindGetter(&Rigidbody::IsVisualizationEnabled, &component), BindSetter(&Rigidbody::SetColliderVisualizationFlag, &component));

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
                            component.SetRigidbodyType(Rigidbody::Type::Static);
                            if (component.IsInitialized())
                            {
                                component.AttachRigidbody(Rigidbody::Type::Static);
                            }
                            component.SetColliderType(Collider::Type::Box);
                            if (component.IsInitialized())
                            {
                                component.AttachCollider(Collider::Type::Box);
                            }
                            component.ApplyTransformScale();
                        }
                        if (currentRigidbodyTypeString == "DYNAMIC")
                        {
                            component.SetRigidbodyType(Rigidbody::Type::Dynamic);
                            if (component.IsInitialized())
                            {
                                component.AttachRigidbody(Rigidbody::Type::Dynamic);
                            }
                            component.SetColliderType(Collider::Type::Box);
                            if (component.IsInitialized())
                            {
                                component.AttachCollider(Collider::Type::Box);
                            }
                            component.ApplyTransformScale();
                        }
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            
            if (component.GetRigidbodyType() == Rigidbody::Type::Dynamic)
            {
                EditorGUI::DrawCheckBox("Enable Gravity", BindGetter(&Rigidbody::IsGravityEnabled, &component), BindSetter(&Rigidbody::SetRigidbodyGravityFlag, &component));
            }
            
            if (component.GetColliderCount() > 0)
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
                                component.SetColliderType(Collider::Type::Sphere);
                                if (component.IsInitialized())
                                {
                                    component.AttachCollider(Collider::Type::Sphere);
                                }
                                component.ApplyTransformScale();
                            }
                            if (currentColliderTypeString == "CAPSULE")
                            {
                                component.SetColliderType(Collider::Type::Capsule);
                                if (component.IsInitialized())
                                {
                                    component.AttachCollider(Collider::Type::Capsule);
                                }
                                component.ApplyTransformScale();
                            }
                            if (currentColliderTypeString == "BOX")
                            {
                                component.SetColliderType(Collider::Type::Box);
                                if (component.IsInitialized())
                                {
                                    component.AttachCollider(Collider::Type::Box);
                                }
                                component.ApplyTransformScale();
                            }
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                
                EditorGUI::DrawVector3Control("Offset Position", BindGetter(&Rigidbody::GetColliderOffsetPosition, &component), BindSetter(&Rigidbody::SetColliderOffsetPosition, &component));
                EditorGUI::DrawVector3Control("Offset Rotation", BindGetter(&Rigidbody::GetColliderOffsetRotation, &component), BindSetter(&Rigidbody::SetColliderOffsetRotation, &component));
                
                if (component.GetColliderType() == Collider::Type::Sphere)
                {
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&Rigidbody::GetSphereRadius, &component), BindSetter(&Rigidbody::SetSphereRadius, &component), 1.0f, 0.0f, FLT_MAX);
                }
                if (component.GetColliderType() == Collider::Type::Capsule)
                {
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&Rigidbody::GetCapsuleRadius, &component), BindSetter(&Rigidbody::SetCapsuleRadius, &component), 1.0f, 0.0f, FLT_MAX);
                    EditorGUI::DrawFloatControl("Half Height", BindGetter(&Rigidbody::GetCapsuleHalfHeight, &component), BindSetter(&Rigidbody::SetCapsuleHalfHeight, &component), 1.0f, 0.0f, FLT_MAX);
                }
                if (component.GetColliderType() == Collider::Type::Box)
                {
                    EditorGUI::DrawVector3Control("Half Extends", BindGetter(&Rigidbody::GetBoxHalfExtents, &component), BindSetter(&Rigidbody::SetBoxHalfExtents, &component),1.0f, 0.0f, FLT_MAX);
                }
                
                EditorGUI::DrawFloatControl("Static Friction", BindGetter(&Rigidbody::GetStaticFriction, &component), BindSetter(&Rigidbody::SetStaticFriction, &component), 1.0f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Dynamic Friction", BindGetter(&Rigidbody::GetDynamicFriction, &component), BindSetter(&Rigidbody::SetDynamicFriction, &component), 1.0f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Restitution", BindGetter(&Rigidbody::GetRestitution, &component), BindSetter(&Rigidbody::SetRestitution, &component), 1.0f, 0.0f, 1.0f);
            }
        }
    };
}
