#include "pch.h"

#include "ComponentDrawer.h"

namespace LevEngine::Editor
{
    class ForceDrawer final : public ComponentDrawer<Force, ForceDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Physics/Force"; }
        
        void DrawContent(Force& component) override
        {
            const Array<String, 4> forceTypeStrings{"Force","Impulse", "Velocity", "Acceleration"};
            EditorGUI::DrawComboBox<Force::Type, 4>("Force Type", forceTypeStrings,
                BindGetter(&Force::GetForceType, &component),
                BindSetter(&Force::SetForceType, &component));
            
            EditorGUI::DrawVector3Control("Linear Force", BindGetter(&Force::GetLinearForce, &component), BindSetter(&Force::SetLinearForce, &component));
            EditorGUI::DrawVector3Control("Angular Force", BindGetter(&Force::GetAngularForce, &component), BindSetter(&Force::SetAngularForce, &component));
        }
    };
}