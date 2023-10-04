#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class RigidbodyDrawer final : public ComponentDrawer<Rigidbody, RigidbodyDrawer>
    {
    protected:
        String GetLabel() const override { return "Rigidbody"; }

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

            GUIUtils::DrawFloatControl("Gravity Scale", component.gravityScale);
            GUIUtils::DrawFloatControl("Mass", component.mass);
            GUIUtils::DrawFloatControl("Elasticity", component.elasticity);
            GUIUtils::DrawFloatControl("Damping", component.damping);
            GUIUtils::DrawFloatControl("Angular Damping", component.angularDamping);

            ImGui::Checkbox("Enabled", &component.enabled);
        }
    };    
}
