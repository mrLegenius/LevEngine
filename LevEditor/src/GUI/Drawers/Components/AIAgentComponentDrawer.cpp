#include "pch.h"

#include <DetourCrowd.h>

#include "ComponentDrawer.h"
#include "AI/Components/AIAgentComponent.h"

namespace LevEngine::Editor
{
    class AIAgentComponentDrawer final : public ComponentDrawer<AIAgentComponent, AIAgentComponentDrawer>
    {
    protected:
        String GetLabel() const override { return "AI/Agent"; }

        void DrawContent(AIAgentComponent& component) override
        {
            const auto agentParams = component.GetAgentParams();
            
            ImGui::Checkbox("Active", &component.isActive);
            
            ImGui::SliderFloat("Radius", &agentParams->radius, 0.1f, 5.0f, "%.2f");
            ImGui::SliderFloat("Height", &agentParams->height, 0.1f, 5.0f, "%.2f");
            ImGui::SliderFloat("MaxAcceleration", &agentParams->maxAcceleration, 0.1f, 50.0f, "%.2f");
            ImGui::SliderFloat("MaxSpeed", &agentParams->maxSpeed, 0.1f, 50.0f, "%.2f");

            ImGui::SliderFloat("CollisionQueryRange", &agentParams->collisionQueryRange, 0.1f, 50.0f, "%.2f");
            ImGui::SliderFloat("PathOptimizationRange", &agentParams->pathOptimizationRange, 0.1f, 150.0f, "%.2f");

            ImGui::SliderFloat("SeparationWeight", &agentParams->separationWeight, 0.1f, 10.0f, "%.2f");

            bool anticipateTurns = agentParams->updateFlags & DT_CROWD_ANTICIPATE_TURNS;
            bool obstacleAvoidance = agentParams->updateFlags & DT_CROWD_OBSTACLE_AVOIDANCE;
            bool separation = agentParams->updateFlags & DT_CROWD_SEPARATION;
            bool optimizeVis = agentParams->updateFlags & DT_CROWD_OPTIMIZE_VIS;
            bool optimizeTopo = agentParams->updateFlags & DT_CROWD_OPTIMIZE_TOPO;
            
            ImGui::Checkbox("Anticipate turns", &anticipateTurns);
            ImGui::Checkbox("Obstacle avoidance", &obstacleAvoidance);
            ImGui::Checkbox("Separation", &separation);
            ImGui::Checkbox("Optimize visibility", &optimizeVis);
            ImGui::Checkbox("Optimize topology", &optimizeTopo);

            agentParams->updateFlags = 0;
            
            if(anticipateTurns)
            {
                agentParams->updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
            }
            if(obstacleAvoidance)
            {
                agentParams->updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
            }
            if(separation)
            {
                agentParams->updateFlags |= DT_CROWD_SEPARATION;
            }
            if(optimizeVis)
            {
                agentParams->updateFlags |= DT_CROWD_OPTIMIZE_VIS;
            }
            if(optimizeTopo)
            {
                agentParams->updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
            }

            const Array<String, 4> obstacleAvoidanceTypes {"Low", "Medium", "High", "Ultra"};
            EditorGUI::DrawComboBox("Obstacle avoidance quality", obstacleAvoidanceTypes, agentParams->obstacleAvoidanceType);
        }
    };
}
