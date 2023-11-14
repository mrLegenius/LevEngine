#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"
#include "Physics/PhysX/PhysicsRigidbody.h"

namespace LevEngine::Editor
{
    class RigidbodyPhysXDrawer final : public ComponentDrawer<PhysicsRigidbody, RigidbodyPhysXDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "RigidbodyPhysX"; }
        
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
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&PhysicsRigidbody::GetSphereColliderRadius, &component), BindSetter(&PhysicsRigidbody::SetSphereColliderRadius, &component), 1.0f, 0.0f, FLT_MAX);
                }
                if (currentColliderTypeString == "CAPSULE")
                {
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&PhysicsRigidbody::GetCapsuleColliderRadius, &component), BindSetter(&PhysicsRigidbody::SetCapsuleColliderRadius, &component), 1.0f, 0.0f, FLT_MAX);
                    EditorGUI::DrawFloatControl("Half Height", BindGetter(&PhysicsRigidbody::GetCapsuleColliderHalfHeight, &component), BindSetter(&PhysicsRigidbody::SetCapsuleColliderHalfHeight, &component), 1.0f, 0.0f, FLT_MAX);
                }
                if (currentColliderTypeString == "BOX")
                {
                    EditorGUI::DrawFloatControl("Half Extend X", BindGetter(&PhysicsRigidbody::GetBoxColliderHalfExtendX, &component), BindSetter(&PhysicsRigidbody::SetBoxColliderHalfExtendX, &component), 1.0f, 0.0f, FLT_MAX);
                    EditorGUI::DrawFloatControl("Half Extend Y", BindGetter(&PhysicsRigidbody::GetBoxColliderHalfExtendY, &component), BindSetter(&PhysicsRigidbody::SetBoxColliderHalfExtendY, &component), 1.0f, 0.0f, FLT_MAX);
                    EditorGUI::DrawFloatControl("Half Extend Z", BindGetter(&PhysicsRigidbody::GetBoxColliderHalfExtendZ, &component), BindSetter(&PhysicsRigidbody::SetBoxColliderHalfExtendZ, &component), 1.0f, 0.0f, FLT_MAX);
                }
                
                EditorGUI::DrawFloatControl("Static Friction", BindGetter(&PhysicsRigidbody::GetStaticFriction, &component), BindSetter(&PhysicsRigidbody::SetStaticFriction, &component), 1.0f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Dynamic Friction", BindGetter(&PhysicsRigidbody::GetDynamicFriction, &component), BindSetter(&PhysicsRigidbody::SetDynamicFriction, &component), 1.0f, 0.0f, FLT_MAX);
                EditorGUI::DrawFloatControl("Restitution", BindGetter(&PhysicsRigidbody::GetRestitution, &component), BindSetter(&PhysicsRigidbody::SetRestitution, &component), 1.0f, 0.0f, 1.0f);
            }
        }
    };
}
