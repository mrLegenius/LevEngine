#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class WaypointMovementComponentDrawer final : public ComponentDrawer<LevEngine::WaypointMovementComponent, WaypointMovementComponentDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "WaypointMovement"; }

        void DrawContent(LevEngine::WaypointMovementComponent& component) override
        {
            GUIUtils::DrawSelectableComponentList<Transform>("Waypoints", component.waypoints, m_itemCurrentIdx);
        }

    private:
        int m_itemCurrentIdx = -1;
    };
}

