#include "pch.h"

#include "ComponentDrawer.h"
#include "Scene/Components/NavMesh/NavMeshComponent.h"

namespace LevEngine::Editor
{
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
            ImGui::SliderFloat("Cell Size", &component.CellSize, 0.1f, 1.0f, "%.2f");
            ImGui::SliderFloat("Cell Height", &component.CellHeight, 0.1f, 1.0f, "%.2f");
            ImGui::Spacing();
            
            ImGui::Separator();
            
            ImGui::Spacing();
            ImGui::Text("Agent");
            ImGui::SliderFloat("Agent Height", &component.AgentHeight, 0.1f, 5.0f, "%.1f");
            ImGui::SliderFloat("Agent Radius", &component.AgentRadius, 0.1f, 5.0f, "%.1f");
            ImGui::SliderFloat("Max Climb", &component.AgentMaxClimb, 0.1f, 5.0f, "%.1f");
            ImGui::SliderFloat("Max Slope", &component.AgentMaxSlope, 0.0f, 5.0f, "%1.0f");
            ImGui::Spacing();

            ImGui::Separator();

            ImGui::Spacing();
            ImGui::Text("Region");
            ImGui::SliderFloat("Min Region Size", &component.RegionMinSize, 0.1f, 150.0f, "%1.0f");
            ImGui::SliderFloat("Merged Region Size", &component.RegionMergeSize, 0.1f, 150.0f, "%1.0f");
            ImGui::Spacing();
            
            ImGui::Separator();
            
            ImGui::Spacing();
            ImGui::Text("Partitioning");
            const auto samplePartitionType = static_cast<SamplePartitionType>(component.PartitionType);
            samplePartitionTypeWatershed = samplePartitionType == SAMPLE_PARTITION_WATERSHED ? true : false;
            samplePartitionTypeMonotone = samplePartitionType == SAMPLE_PARTITION_MONOTONE ? true : false;
            samplePartitionTypeLayers = samplePartitionType == SAMPLE_PARTITION_LAYERS ? true : false;
            if (ImGui::Checkbox("Watershed", &samplePartitionTypeWatershed))
            {
                component.PartitionType = SAMPLE_PARTITION_WATERSHED;
            }
            if (ImGui::Checkbox("Monotone", &samplePartitionTypeMonotone))
            {
                component.PartitionType = SAMPLE_PARTITION_MONOTONE;
            }
            if (ImGui::Checkbox("Layers", &samplePartitionTypeLayers))
            {
                component.PartitionType = SAMPLE_PARTITION_LAYERS;
            }
            ImGui::Spacing();

            ImGui::Separator();

            ImGui::Spacing();
            ImGui::Text("Filtering");
            ImGui::Checkbox("Low Hanging Obstacles", &component.FilterLowHangingObstacles);
            ImGui::Checkbox("Ledge Spans", &component.FilterLedgeSpans);
            ImGui::Checkbox("Walkable Low Height Spans", &component.FilterWalkableLowHeightSpans);
            ImGui::Spacing();
            
            ImGui::Separator();

            ImGui::Spacing();
            ImGui::Text("Polygonization");
            ImGui::SliderFloat("Max Edge Length", &component.EdgeMaxLen, 0.0f, 50.0f, "%1.0f");
            ImGui::SliderFloat("Max Edge Error", &component.EdgeMaxError, 0.1f, 3.0f, "%.1f");
            ImGui::SliderFloat("Verts Per Poly", &component.VertsPerPoly, 3.0f, 12.0f, "%.1f");
            ImGui::Spacing();

            ImGui::Separator();

            ImGui::Spacing();
            ImGui::Text("Detail mesh");
            ImGui::SliderFloat("Sample Distance", &component.DetailSampleDist, 0.0f, 16.0f, "%1.0f");
            ImGui::SliderFloat("Max Sample Error Distance", &component.DetailSampleMaxError, 0.0f, 16.0f, "%1.0f");
            ImGui::Spacing();

            ImGui::Separator();

            ImGui::Spacing();
            ImGui::Checkbox("Keep Intermediate Results", &component.KeepIntermediateResults);
            ImGui::Spacing();
        }

        bool samplePartitionTypeWatershed = false;
        bool samplePartitionTypeMonotone = false;
        bool samplePartitionTypeLayers = false;
    };	
}
