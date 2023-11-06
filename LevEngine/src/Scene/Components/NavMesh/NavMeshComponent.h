#pragma once
#include "RecastDetour/Recast/Recast.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(NavMeshComponent);
	
    struct NavMeshComponent
    {
        NavMeshComponent() = default;
        NavMeshComponent(const NavMeshComponent&) = default;
        
        void Build();

        rcConfig m_Config;

        float m_CellSize = 0.3f;
        float m_CellHeight = 0.2f;
        float m_AgentHeight = 2.0f;
        float m_AgentRadius = 0.6f;
        float m_AgentMaxClimb = 0.9f;
        float m_AgentMaxSlope = 45.0f;
        float m_RegionMinSize = 8.0f;
        float m_RegionMergeSize = 8.0f;
        float m_EdgeMaxLen = 12.0f;
        float m_EdgeMaxError = 1.3f;
        float m_VertsPerPoly = 6.0f;
        float m_DetailSampleDist = 8.0f;
        float m_DetailSampleMaxError = 1.0f;
        int m_PartitionType = 0;
    };
}
