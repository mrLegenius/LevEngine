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
            EditorGUI::DrawSelectableComponentList<AIAgentComponent>("Agents", component.agentsEntities, m_agentCurrentIdx);
            EditorGUI::DrawSelectableComponent<NavMeshComponent>("Nav mesh", component.navMesh, m_navMeshWasSelected,
                m_navMeshIsSelected, m_navMeshSelectableIdx);
            
            // float velBias;
            // float weightDesVel;
            // float weightCurVel;
            // float weightSide;
            // float weightToi;
            // float horizTime;
            // unsigned char gridSize;	///< grid
            // unsigned char adaptiveDivs;	///< adaptive
            // unsigned char adaptiveRings;	///< adaptive
            // unsigned char adaptiveDepth;	///< adaptive
        }
        
    private:
        
        int m_agentCurrentIdx = -1;
        bool m_navMeshWasSelected = false;
        bool m_navMeshIsSelected = false;
        int m_navMeshSelectableIdx = 0;
    };
}
