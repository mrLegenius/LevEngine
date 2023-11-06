#include "pch.h"

#include "ComponentDrawer.h"
#include "Scene/Components/NavMesh/NavMeshableComponent.h"

namespace LevEngine::Editor
{
    class NavMeshableComponentDrawer final : public ComponentDrawer<NavMeshableComponent, NavMeshableComponentDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "NavMeshable"; }
        String GetAddMenuPath() const override { return "NavMesh"; }
    
        void DrawContent(NavMeshableComponent& component) override
        {
            ImGui::Checkbox("Use in navmesh building", &component.useInNavMesh);
        }
    };	
}
