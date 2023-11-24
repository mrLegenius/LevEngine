﻿#include "pch.h"

#include "ComponentDrawer.h"

namespace LevEngine::Editor
{
    class RigidbodyDrawer final : public ComponentDrawer<Rigidbody, RigidbodyDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Physics/Rigidbody"; }
        
        void DrawContent(Rigidbody& component) override
        {
            EditorGUI::DrawCheckBox("Draw Collider", BindGetter(&Rigidbody::IsVisualizationEnabled, &component), BindSetter(&Rigidbody::EnableVisualization, &component));

            const Array<String, 2> rigidbodyTypeStrings{"Static","Dynamic"};
            EditorGUI::DrawComboBox<Rigidbody::Type, 2>("Rigidbody Type", rigidbodyTypeStrings,
                BindGetter(&Rigidbody::GetRigidbodyType, &component), BindSetter(&Rigidbody::SetRigidbodyType, &component));
            
            if (component.GetRigidbodyType() == Rigidbody::Type::Dynamic)
            {
                EditorGUI::DrawCheckBox("Enable Gravity", BindGetter(&Rigidbody::IsGravityEnabled, &component), BindSetter(&Rigidbody::EnableGravity, &component));
            }
            
            if (component.GetColliderCount() > 0)
            {
                const Array<String, 3> colliderTypeStrings{"Sphere","Capsule", "Box"};
                EditorGUI::DrawComboBox<Collider::Type, 3>("Collider Type", colliderTypeStrings,
                    BindGetter(&Rigidbody::GetColliderType, &component), BindSetter(&Rigidbody::SetColliderType, &component));
                
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
