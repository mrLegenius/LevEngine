#pragma once
#pragma once
#include <DetourCrowd.h>
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

        void Init(AIAgentCrowdComponent* crowdComponent, int agentIndex);
        
        bool isActive = false; 
        
        dtCrowdAgentParams* GetAgentParams() const;
        void SetAgentParams(dtCrowdAgentParams* params);

        void SetMoveTarget(Vector3 targetPos);


    private:

        bool m_initialized = false;
        
        int m_agentIndex;
        
        AIAgentCrowdComponent* m_crowd;
        dtCrowdAgent* m_agent;
        dtCrowdAgentParams* m_agentParams;
    };
}
