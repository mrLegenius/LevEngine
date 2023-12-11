#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"
#include "Assets/TextureAsset.h"

namespace LevEngine::Editor
{
    class SkyboxComponentDrawer final : public ComponentDrawer<SkyboxRendererComponent, SkyboxComponentDrawer>
    {
    protected:
        String GetLabel() const override { return "Renderers/Skybox Renderer"; }

        void DrawContent(SkyboxRendererComponent& component) override
        {
            EditorGUI::DrawAsset<TextureAsset>("Skybox Texture", component.SkyboxTexture);
        }
    };
}
