#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class MeshRendererDrawer final : public ComponentDrawer<MeshRendererComponent, MeshRendererDrawer>
    {
    protected:
        String GetLabel() const override { return "Renderers/Mesh Renderer"; }

        void DrawContent(MeshRendererComponent& component) override
        {
            ImGui::Checkbox("Enabled", &component.enabled);
            
            EditorGUI::DrawAsset<MeshAsset>("Mesh", component.mesh);
            EditorGUI::DrawAsset<MaterialAsset>("Material", component.material);

            ImGui::Checkbox("Cast shadows", &component.castShadow);
        }
    };
}
