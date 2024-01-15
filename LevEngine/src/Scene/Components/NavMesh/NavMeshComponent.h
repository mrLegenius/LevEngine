#pragma once
#include "Recast.h"
#include "Scene/Entity.h"
#include "Scene/Components/TypeParseTraits.h"

class dtNavMeshQuery;
class dtNavMesh;

namespace LevEngine
{
    enum SamplePartitionType : int
    {
        SamplePartitionWatershed = 0,
        SamplePartitionMonotone = 1,
        SamplePartitionLayers = 2
    };
    
    REGISTER_PARSE_TYPE(NavMeshComponent);
	
    struct NavMeshComponent
    {
        NavMeshComponent();
        NavMeshComponent(const NavMeshComponent&) = default;

        void ConstructComponent(Entity entity);
        static void OnConstruct(entt::registry& registry, entt::entity entity);
        
        void Build();

        bool IsBuilded = false;
        
        bool buildOnStart = false;
        const rcPolyMesh& GetPolyMesh() const;
        
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
        SamplePartitionType PartitionType = SamplePartitionWatershed;
        bool KeepIntermediateResults = false;
        bool FilterLowHangingObstacles = false;
        bool FilterLedgeSpans = false;
        bool FilterWalkableLowHeightSpans = false;
        
    private:

        Entity m_self;
        
        void Cleanup();
        
        dtNavMesh* m_NavMesh = nullptr;
        dtNavMeshQuery* m_NavQuery = nullptr;
        
        rcHeightfield* m_Solid = nullptr;
        rcCompactHeightfield* m_CompactHeightfield = nullptr;
        unsigned char* m_TriangleAreas = nullptr;
        rcContourSet* m_ContourSet = nullptr;
        rcPolyMesh* m_PolyMesh = nullptr;
        rcPolyMeshDetail* m_PolyMeshDetail = nullptr;
        
        rcContext* m_Context = new rcContext();
    };
}
