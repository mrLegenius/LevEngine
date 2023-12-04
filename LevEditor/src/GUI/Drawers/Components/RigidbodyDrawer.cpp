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
            EditorGUI::DrawCheckBox("Draw Collider", BindGetter(&Rigidbody::IsVisualizationEnabled, &component), BindSetter(&Rigidbody::EnableVisualization, &component));

            const Array<String, 2> rigidbodyTypeStrings{"Static","Dynamic"};
            EditorGUI::DrawComboBox<Rigidbody::Type, 2>("Rigidbody Type", rigidbodyTypeStrings,
                BindGetter(&Rigidbody::GetRigidbodyType, &component), BindSetter(&Rigidbody::SetRigidbodyType, &component));
            
            if (component.GetRigidbodyType() == Rigidbody::Type::Dynamic)
            {
                EditorGUI::DrawCheckBox("Enable Kinematic", BindGetter(&Rigidbody::IsKinematicEnabled, &component), BindSetter(&Rigidbody::EnableKinematic, &component));
                
                EditorGUI::DrawCheckBox("Enable Gravity", BindGetter(&Rigidbody::IsGravityEnabled, &component), BindSetter(&Rigidbody::EnableGravity, &component));
                
                EditorGUI::DrawFloatControl("Mass", BindGetter(&Rigidbody::GetMass, &component), BindSetter(&Rigidbody::SetMass, &component));
                EditorGUI::DrawVector3Control("Center Of Mass", BindGetter(&Rigidbody::GetCenterOfMass, &component), BindSetter(&Rigidbody::SetCenterOfMass, &component));
                EditorGUI::DrawVector3Control("Diagonal Inertia", BindGetter(&Rigidbody::GetInertiaTensor, &component), BindSetter(&Rigidbody::SetInertiaTensor, &component));

                EditorGUI::DrawFloatControl("Max Linear Velocity", BindGetter(&Rigidbody::GetMaxLinearVelocity, &component), BindSetter(&Rigidbody::SetMaxLinearVelocity, &component));
                EditorGUI::DrawFloatControl("Max Angular Velocity", BindGetter(&Rigidbody::GetMaxAngularVelocity, &component), BindSetter(&Rigidbody::SetMaxAngularVelocity, &component));
                
                EditorGUI::DrawFloatControl("Linear Damping", BindGetter(&Rigidbody::GetLinearDamping, &component), BindSetter(&Rigidbody::SetLinearDamping, &component));
                EditorGUI::DrawFloatControl("Angular Damping", BindGetter(&Rigidbody::GetAngularDamping, &component), BindSetter(&Rigidbody::SetAngularDamping, &component));
                
                EditorGUI::DrawCheckBox("Lock X Pos Axis", BindGetter(&Rigidbody::IsPosAxisXLocked, &component), BindSetter(&Rigidbody::LockPosAxisX, &component));
                EditorGUI::DrawCheckBox("Lock Y Pos Axis", BindGetter(&Rigidbody::IsPosAxisYLocked, &component), BindSetter(&Rigidbody::LockPosAxisY, &component));
                EditorGUI::DrawCheckBox("Lock Z Pos Axis", BindGetter(&Rigidbody::IsPosAxisZLocked, &component), BindSetter(&Rigidbody::LockPosAxisZ, &component));

                EditorGUI::DrawCheckBox("Lock X Rot Axis", BindGetter(&Rigidbody::IsRotAxisXLocked, &component), BindSetter(&Rigidbody::LockRotAxisX, &component));
                EditorGUI::DrawCheckBox("Lock Y Rot Axis", BindGetter(&Rigidbody::IsRotAxisYLocked, &component), BindSetter(&Rigidbody::LockRotAxisY, &component));
                EditorGUI::DrawCheckBox("Lock Z Rot Axis", BindGetter(&Rigidbody::IsRotAxisZLocked, &component), BindSetter(&Rigidbody::LockRotAxisZ, &component));
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
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&Rigidbody::GetSphereRadius, &component), BindSetter(&Rigidbody::SetSphereRadius, &component));
                }
                if (component.GetColliderType() == Collider::Type::Capsule)
                {
                    EditorGUI::DrawFloatControl("Radius", BindGetter(&Rigidbody::GetCapsuleRadius, &component), BindSetter(&Rigidbody::SetCapsuleRadius, &component));
                    EditorGUI::DrawFloatControl("Half Height", BindGetter(&Rigidbody::GetCapsuleHalfHeight, &component), BindSetter(&Rigidbody::SetCapsuleHalfHeight, &component));
                }
                if (component.GetColliderType() == Collider::Type::Box)
                {
                    EditorGUI::DrawVector3Control("Half Extends", BindGetter(&Rigidbody::GetBoxHalfExtents, &component), BindSetter(&Rigidbody::SetBoxHalfExtents, &component));
                }
                
                EditorGUI::DrawFloatControl("Static Friction", BindGetter(&Rigidbody::GetStaticFriction, &component), BindSetter(&Rigidbody::SetStaticFriction, &component));
                EditorGUI::DrawFloatControl("Dynamic Friction", BindGetter(&Rigidbody::GetDynamicFriction, &component), BindSetter(&Rigidbody::SetDynamicFriction, &component));
                EditorGUI::DrawFloatControl("Restitution", BindGetter(&Rigidbody::GetRestitution, &component), BindSetter(&Rigidbody::SetRestitution, &component));
            }
        }
    };
}
