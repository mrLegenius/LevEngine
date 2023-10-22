#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class RigidbodyPhysXDrawer final : public ComponentDrawer<RigidbodyPhysX, RigidbodyPhysXDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "RigidbodyPhysX"; }
        
        void DrawContent(RigidbodyPhysX& component) override
        {
            EditorGUI::DrawCheckBox("Draw Collider", BindGetter(&RigidbodyPhysX::GetColliderVisualizationFlag, &component), BindSetter(&RigidbodyPhysX::SetColliderVisualizationFlag, &component));
            
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
                EditorGUI::DrawCheckBox("Enable Gravity", BindGetter(&RigidbodyPhysX::GetRigidbodyGravityFlag, &component), BindSetter(&RigidbodyPhysX::SetRigidbodyGravityFlag, &component));
            }
            
            if (component.GetRigidbody()->getNbShapes() > 0)
            {
                const char* colliderTypeStrings[] = { "SPHERE", "PLANE", "CAPSULE", "BOX" };
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
                            {;
                                component.AttachCollider(ColliderType::Sphere);
                            }
                            if (component.GetRigidbodyType() == RigidbodyType::Static)
                            {
                            }
                            if (currentColliderTypeString == "PLANE")
                            {
                                if (component.GetRigidbodyType() == RigidbodyType::Static)
                                {
                                    component.AttachCollider(ColliderType::Plane);
                                }
                                if (component.GetRigidbodyType() == RigidbodyType::Dynamic)
                                {
                                    component.AttachCollider(ColliderType::Box);
                                }
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
                if (currentColliderTypeString == "SPHERE")
                {
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&RigidbodyPhysX::GetSphereColliderRadius, &component), BindSetter(&RigidbodyPhysX::SetSphereColliderRadius, &component), 1.0f, 0.0f, FLT_MAX);
                }
                if (currentColliderTypeString == "CAPSULE")
                {
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&RigidbodyPhysX::GetCapsuleColliderRadius, &component), BindSetter(&RigidbodyPhysX::SetCapsuleColliderRadius, &component), 1.0f, 0.0f, FLT_MAX);
                    EditorGUI::DrawFloatControl("Half Height", BindGetter(&RigidbodyPhysX::GetCapsuleColliderHalfHeight, &component), BindSetter(&RigidbodyPhysX::SetCapsuleColliderHalfHeight, &component), 1.0f, 0.0f, FLT_MAX);
                }
                if (currentColliderTypeString == "BOX")
                {
                    EditorGUI::DrawFloatControl("Half Extend X", BindGetter(&RigidbodyPhysX::GetBoxColliderHalfExtendX, &component), BindSetter(&RigidbodyPhysX::SetBoxColliderHalfExtendX, &component), 1.0f, 0.0f, FLT_MAX);
                    EditorGUI::DrawFloatControl("Half Extend Y", BindGetter(&RigidbodyPhysX::GetBoxColliderHalfExtendY, &component), BindSetter(&RigidbodyPhysX::SetBoxColliderHalfExtendY, &component), 1.0f, 0.0f, FLT_MAX);
                    EditorGUI::DrawFloatControl("Half Extend Z", BindGetter(&RigidbodyPhysX::GetBoxColliderHalfExtendZ, &component), BindSetter(&RigidbodyPhysX::SetBoxColliderHalfExtendZ, &component), 1.0f, 0.0f, FLT_MAX);
                }
                
                EditorGUI::DrawFloatControl("Static Friction", BindGetter(&RigidbodyPhysX::GetStaticFriction, &component), BindSetter(&RigidbodyPhysX::SetStaticFriction, &component), 1.0f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Dynamic Friction", BindGetter(&RigidbodyPhysX::GetDynamicFriction, &component), BindSetter(&RigidbodyPhysX::SetDynamicFriction, &component), 1.0f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Restitution", BindGetter(&RigidbodyPhysX::GetRestitution, &component), BindSetter(&RigidbodyPhysX::SetRestitution, &component), 1.0f, 0.0f, 1.0f);
            }
        }
    };
}