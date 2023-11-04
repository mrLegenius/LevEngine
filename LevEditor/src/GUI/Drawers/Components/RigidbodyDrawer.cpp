#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class RigidbodyDrawer final : public ComponentDrawer<Rigidbody, RigidbodyDrawer>
    {
    protected:
        String GetLabel() const override { return "Rigidbody"; }
        String GetAddMenuPath() const override { return "Physics"; }

        void DrawContent(Rigidbody& component) override
        {
            const char* bodyTypeStrings[] = { "Static", "Kinematic", "Dynamic" };
            const char* currentBodyTypeString = bodyTypeStrings[static_cast<int>(component.bodyType)];

            if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
            {
                for (int i = 0; i < 3; i++)
                {
                    const bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
                    if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
                    {
                        currentBodyTypeString = bodyTypeStrings[i];
                        component.bodyType = static_cast<BodyType>(i);
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            EditorGUI::DrawFloatControl("Gravity Scale", component.gravityScale);
            EditorGUI::DrawFloatControl("Mass", component.mass);
            EditorGUI::DrawFloatControl("Elasticity", component.elasticity);
            EditorGUI::DrawFloatControl("Damping", component.damping);
            EditorGUI::DrawFloatControl("Angular Damping", component.angularDamping);

            ImGui::Checkbox("Enabled", &component.enabled);
        }
    };    
}
