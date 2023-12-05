#include "pch.h"

#include "ComponentDrawer.h"

namespace LevEngine::Editor
{
    class ConstantForceDrawer final : public ComponentDrawer<ConstantForce, ConstantForceDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Physics/Constant Force"; }
        
        void DrawContent(ConstantForce& component) override
        {
            EditorGUI::DrawVector3Control("Force", BindGetter(&ConstantForce::GetForce, &component), BindSetter(&ConstantForce::SetForce, &component));
            EditorGUI::DrawVector3Control("Torque", BindGetter(&ConstantForce::GetTorque, &component), BindSetter(&ConstantForce::SetTorque, &component));
        }
    };
}