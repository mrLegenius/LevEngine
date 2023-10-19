#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class WaypointMovementComponentDrawer final : public ComponentDrawer<LevEngine::WaypointMovementComponent, WaypointMovementComponentDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "WaypointMovement"; }

        void DrawContent(LevEngine::WaypointMovementComponent& component) override
        {
            EditorGUI::DrawSelectableComponentList<Transform>("Waypoints", component.entities, m_itemCurrentIdx);
        }

    private:
        int m_itemCurrentIdx = -1;
    };
}

