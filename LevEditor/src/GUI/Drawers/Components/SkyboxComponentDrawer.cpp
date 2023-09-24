#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class SkyboxComponentDrawer final : public ComponentDrawer<SkyboxRendererComponent, SkyboxComponentDrawer>
    {
    protected:
        String GetLabel() const override { return "Skybox Renderer"; }

        void DrawContent(SkyboxRendererComponent& component) override
        {
            GUIUtils::DrawAsset<SkyboxAsset>("Skybox", &component.skybox);
        }
    };
}
