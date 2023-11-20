#pragma once
#include "RecastDetour/Detour/DetourNavMesh.h"
#include "RecastDetour/Recast/Recast.h"
#include "RecastDetour/Detour/DetourNavMeshQuery.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    enum SamplePartitionType : int
    {
        SAMPLE_PARTITION_WATERSHED = 0,
        SAMPLE_PARTITION_MONOTONE = 1,
        SAMPLE_PARTITION_LAYERS = 2
    };
    
    REGISTER_PARSE_TYPE(NavMeshComponent);
	
    struct NavMeshComponent
    {
        NavMeshComponent();
        NavMeshComponent(const NavMeshComponent&) = default;
        
        void Build();
        
        float CellSize = 0.3f;
        float CellHeight = 0.2f;
        float AgentHeight = 2.0f;
        float AgentRadius = 0.6f;
        float AgentMaxClimb = 0.9f;
        float AgentMaxSlope = 45.0f;
        float RegionMinSize = 8.0f;
        float RegionMergeSize = 8.0f;
        float EdgeMaxLen = 12.0f;
        float EdgeMaxError = 1.3f;
        float VertsPerPoly = 6.0f;
        float DetailSampleDist = 8.0f;
        float DetailSampleMaxError = 1.0f;
        int PartitionType = 0;
        bool KeepIntermediateResults;
        bool FilterLowHangingObstacles;
        bool FilterLedgeSpans;
        bool FilterWalkableLowHeightSpans;
    private:

        void Cleanup();
        
        dtNavMesh* m_NavMesh = nullptr;
        dtNavMeshQuery* m_NavQuery = nullptr;
        rcConfig m_Config;
        
        rcHeightfield* m_Solid = nullptr;
        rcCompactHeightfield* m_CompactHeightfield = nullptr;
        unsigned char* m_TriangleAreas = nullptr;
        rcContourSet* m_ContourSet = nullptr;
        rcPolyMesh* m_PolyMesh = nullptr;
        rcPolyMeshDetail* m_PolyMeshDetail = nullptr;
        
        rcContext* m_Context = new rcContext();
    };
}
