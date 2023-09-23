#include "pch.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class WaypointMovementComponentDrawer final : public LevEngine::ComponentDrawer<LevEngine::WaypointMovementComponent, WaypointMovementComponentDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "WaypointMovement"; }

        void DrawContent(LevEngine::WaypointMovementComponent& component) override
        {
            GUIUtils::DrawSelectableComponentList("Waypoints", component.waypoints, m_itemCurrentIdx);
        }

    private:
        int m_itemCurrentIdx = -1;
    };
}

