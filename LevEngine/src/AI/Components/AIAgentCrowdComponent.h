#pragma once

#include <DetourCrowd.h>

#include "EASTL/stack.h"
#include "Scene/Entity.h"
#include "Scene/Components/TypeParseTraits.h"

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

        void Init(Entity crowdEntity);
       
        void Update(float deltaTime);
        bool IsInitialized() const;
        bool IsInitializationFailed() const;

        void UpdateAgentsPosition();
        void SetMoveTarget(int agentIndex, Vector3 targetPos);
        void AddAgent(Entity agentEntity);
        void RemoveAgent(Entity agentEntity);
        
        Vector<Entity> initialAgentsEntities;
        
        Entity navMesh;
        Entity target;

    private:

        Vector<Entity> agents;

        Entity m_selfEntity;
        
        bool m_isInitialized = false;
        bool m_isInitializationFailed = false;
        

        eastl::stack<int> m_agentIndexesPool;
        void RegisterDefaultObstacleAvoidanceProfiles();
        
        dtCrowd* m_crowd;
        dtNavMesh* m_navMesh;
        dtNavMeshQuery* m_navMeshQuery;

        dtPolyRef m_targetRef;

        dtCrowdAgentDebugInfo* m_crowdAgentDebugInfo;

        int MAX_AGENTS_COUNT = 100;
    };
}
