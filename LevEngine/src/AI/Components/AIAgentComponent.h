#pragma once
#pragma once
#include <DetourCrowd.h>

#include "Scene/Entity.h"
#include "Scene/Components/TypeParseTraits.h"

class dtNavMeshQuery;
class dtNavMesh;

namespace LevEngine
{
    struct AIAgentCrowdComponent;

    REGISTER_PARSE_TYPE(AIAgentComponent);
	
    struct AIAgentComponent
    {
        AIAgentComponent();
        AIAgentComponent(const AIAgentComponent&) = default;

        void Init(Entity crowd, Entity agent, int agentIndex);
        bool IsInitialized();
        static void OnDestroy(entt::registry& registry, entt::entity entity);
        void OnComponentDestroy();
        
        
        bool isActive = false; 
        
        dtCrowdAgentParams* GetAgentParams() const;
        void SetAgentParams(dtCrowdAgentParams* params);

        void SetMoveTarget(Vector3 targetPos);


    private:

        bool m_initialized = false;
        
        int m_agentIndex;
        
        Entity m_crowd;
        Entity m_selfEntity;
        
        dtCrowdAgent* m_agent;
        dtCrowdAgentParams* m_agentParams;
    };
}
