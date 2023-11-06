#include "pch.h"

#include "ComponentDrawer.h"
#include "Scene/Components/NavMesh/NavMeshComponent.h"

namespace LevEngine::Editor
{
    enum SamplePartitionType : int
    {
        SAMPLE_PARTITION_WATERSHED = 0,
        SAMPLE_PARTITION_MONOTONE = 1,
        SAMPLE_PARTITION_LAYERS = 2
    };
    
    class NavMeshComponentDrawer final : public ComponentDrawer<NavMeshComponent, NavMeshComponentDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "NavMesh"; }
        String GetAddMenuPath() const override { return "NavMesh"; }
        
        void DrawContent(NavMeshComponent& component) override
        {
            ImGui::Spacing();
            if(ImGui::Button("Build", ImVec2(60.0f, 25.0f)))
            {
                component.Build();
            }
            ImGui::Spacing();
            
            ImGui::Separator();
            
            ImGui::Spacing();
            ImGui::Text("Rasterization");
            ImGui::SliderFloat("Cell size", &component.m_CellSize, 0.1f, 1.0f, "%.2f");
            ImGui::SliderFloat("Cell height", &component.m_CellHeight, 0.1f, 1.0f, "%.2f");
            ImGui::Spacing();
            
            ImGui::Separator();
            
            ImGui::Spacing();
            ImGui::Text("Agent");
            ImGui::SliderFloat("Agent height", &component.m_AgentHeight, 0.1f, 5.0f, "%.1f");
            ImGui::SliderFloat("Agent radius", &component.m_AgentRadius, 0.1f, 5.0f, "%.1f");
            ImGui::SliderFloat("Max climb", &component.m_AgentMaxClimb, 0.1f, 5.0f, "%.1f");
            ImGui::SliderFloat("Max slope", &component.m_AgentMaxSlope, 0.0f, 5.0f, "%1.0f");
            ImGui::Spacing();

            ImGui::Separator();

            ImGui::Spacing();
            ImGui::Text("Region");
            ImGui::SliderFloat("Min region size", &component.m_RegionMinSize, 0.1f, 150.0f, "%1.0f");
            ImGui::SliderFloat("Merged region size", &component.m_RegionMergeSize, 0.1f, 150.0f, "%1.0f");
            ImGui::Spacing();
            
            ImGui::Separator();
            
            ImGui::Spacing();
            ImGui::Text("Partitioning");
            const auto samplePartitionType = static_cast<SamplePartitionType>(component.m_PartitionType);
            samplePartitionTypeWatershed = samplePartitionType == SAMPLE_PARTITION_WATERSHED ? true : false;
            samplePartitionTypeMonotone = samplePartitionType == SAMPLE_PARTITION_MONOTONE ? true : false;
            samplePartitionTypeLayers = samplePartitionType == SAMPLE_PARTITION_LAYERS ? true : false;
            if (ImGui::Checkbox("Watershed", &samplePartitionTypeWatershed))
            {
                component.m_PartitionType = SAMPLE_PARTITION_WATERSHED;
            }
            if (ImGui::Checkbox("Monotone", &samplePartitionTypeMonotone))
            {
                component.m_PartitionType = SAMPLE_PARTITION_MONOTONE;
            }
            if (ImGui::Checkbox("Layers", &samplePartitionTypeLayers))
            {
                component.m_PartitionType = SAMPLE_PARTITION_LAYERS;
            }
            ImGui::Spacing();
            
            ImGui::Separator();

            ImGui::Spacing();
            ImGui::Text("Polygonization");
            ImGui::SliderFloat("Max edge length", &component.m_EdgeMaxLen, 0.0f, 50.0f, "%1.0f");
            ImGui::SliderFloat("Max edge error", &component.m_EdgeMaxError, 0.1f, 3.0f, "%.1f");
            ImGui::SliderFloat("Verts per poly", &component.m_VertsPerPoly, 3.0f, 12.0f, "%.1f");
            ImGui::Spacing();

            ImGui::Separator();

            ImGui::Spacing();
            ImGui::Text("Detail mesh");
            ImGui::SliderFloat("Sample distance", &component.m_EdgeMaxError, 0.0f, 16.0f, "%1.0f");
            ImGui::SliderFloat("Max sample error distance", &component.m_DetailSampleMaxError, 0.0f, 16.0f, "%1.0f");
            ImGui::Spacing();
        }

        bool samplePartitionTypeWatershed = false;
        bool samplePartitionTypeMonotone = false;
        bool samplePartitionTypeLayers = false;
    };	
}
