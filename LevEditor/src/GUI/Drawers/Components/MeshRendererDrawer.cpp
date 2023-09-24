#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class MeshRendererDrawer final : public ComponentDrawer<MeshRendererComponent, MeshRendererDrawer>
    {
    protected:
        String GetLabel() const override { return "Mesh Renderer"; }

        void DrawContent(MeshRendererComponent& component) override
        {
            GUIUtils::DrawAsset<MeshAsset>("Mesh", &component.mesh);
            GUIUtils::DrawAsset<MaterialAsset>("Material", &component.material);

            ImGui::Checkbox("Cast shadows", &component.castShadow);
        }
    };
}
