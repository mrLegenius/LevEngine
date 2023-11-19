#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class RigidbodyDrawer final : public ComponentDrawer<Rigidbody, RigidbodyDrawer>
    {
    protected:
        String GetLabel() const override { return "Physics/Rigidbody"; }

        void DrawContent(Rigidbody& component) override
        {
            EditorGUI::DrawComboBox("Body Type", BodyTypeStrings, component.bodyType);
            EditorGUI::DrawFloatControl("Gravity Scale", component.gravityScale);
            EditorGUI::DrawFloatControl("Mass", component.mass);
            EditorGUI::DrawFloatControl("Elasticity", component.elasticity);
            EditorGUI::DrawFloatControl("Damping", component.damping);
            EditorGUI::DrawFloatControl("Angular Damping", component.angularDamping);

            ImGui::Checkbox("Enabled", &component.enabled);
        }
    };    
}
