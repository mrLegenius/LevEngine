#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class SkyboxComponentDrawer final : public ComponentDrawer<SkyboxRendererComponent, SkyboxComponentDrawer>
    {
    protected:
        String GetLabel() const override { return "Skybox Renderer"; }

        void DrawContent(SkyboxRendererComponent& component) override
        {
            EditorGUI::DrawAsset<SkyboxAsset>("Skybox", component.skybox);
        }
    };
}
