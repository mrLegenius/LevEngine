#include "pch.h"

#include "ComponentDrawer.h"
#include "AI/Components/AIAgentComponent.h"
#include "AI/Components/AIAgentCrowdComponent.h"
#include "GUI/EditorGUI.h"
#include "Scene/Components/NavMesh/NavMeshComponent.h"

namespace LevEngine::Editor
{
    class AIAgentCrowdComponentDrawer final : public ComponentDrawer<AIAgentCrowdComponent, AIAgentCrowdComponentDrawer>
    {
    protected:
        String GetLabel() const override { return "AI/Crowd"; }

        void DrawContent(AIAgentCrowdComponent& component) override
        {
            EditorGUI::DrawSelectableComponentList<AIAgentComponent>("Agents", component.initialAgentsEntities, m_agentCurrentIdx);
            EditorGUI::DrawSelectableComponent<NavMeshComponent>("Nav mesh", component.navMesh, m_navMeshWasSelected,
                m_navMeshIsSelected, m_navMeshSelectableIdx);
        }
        
    private:
        
        int m_agentCurrentIdx = -1;
        
        bool m_navMeshWasSelected = false;
        bool m_navMeshIsSelected = false;
        int m_navMeshSelectableIdx = 0;

        bool m_targetWasSelected = false;
        bool m_targetIsSelected = false;
        int m_targetSelectableIdx = 0;
    };
}
