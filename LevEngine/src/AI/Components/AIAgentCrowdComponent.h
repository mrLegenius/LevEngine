#pragma once

#include <DetourCrowd.h>

#include "Scene/Entity.h"
#include "Scene/Components/TypeParseTraits.h"

class dtCrowd;

namespace LevEngine
{
    enum class ObstacleAvoidanceQuality : int
    {
        Low,
        Medium,
        High,
        Ultra,
    };
    
    REGISTER_PARSE_TYPE(AIAgentCrowdComponent);
	
    struct AIAgentCrowdComponent
    {
        AIAgentCrowdComponent();
        AIAgentCrowdComponent(const AIAgentCrowdComponent&) = default;

        void Init();
       
        void Update(const float deltaTime);
        bool IsInitialized() const;

        void UpdateAgentsPosition();
        void SetMoveTarget(int agentIndex, Vector3 targetPos);

        Vector<Entity> agentsEntities;
        Entity navMesh;
        int OnConstruct;

    private:

        bool m_isInitialized = false;
        
        void AddAgent(const Entity& agentEntity);
        void RegisterDefaultObstacleAvoidanceProfiles();
        
        dtCrowd* m_crowd;
        dtNavMesh* m_navMesh;
        dtNavMeshQuery* m_navMeshQuery;

        dtPolyRef m_targetRef;

        dtCrowdAgentDebugInfo* m_crowdAgentDebugInfo;
    };
}
