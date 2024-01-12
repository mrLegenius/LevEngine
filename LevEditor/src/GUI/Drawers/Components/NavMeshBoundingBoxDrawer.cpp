#include "pch.h"

#include "ComponentDrawer.h"
#include "Scene/Components/NavMesh/NavMeshBoundingBox.h"

namespace LevEngine::Editor
{
    class NavMeshBoundingBoxDrawer final : public ComponentDrawer<NavMeshBoundingBox, NavMeshBoundingBoxDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Nav mesh/Nav mesh bounding box"; }
    
        void DrawContent(NavMeshBoundingBox& component) override
        {
            auto positionOffset = component.GetPositionOffset();
            if (EditorGUI::DrawVector3Control("PositionOffset", positionOffset))
                component.SetPositionOffset(positionOffset);
            auto boxHalfExtents = component.GetBoxHalfExtents();
            if (EditorGUI::DrawVector3Control("BoxHalfExtents", boxHalfExtents))
                component.SetBoxHalfExtents(boxHalfExtents);
        }
    };	
}
