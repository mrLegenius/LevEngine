#pragma once
#pragma once
#include <DetourCrowd.h>
#include "Scene/Components/TypeParseTraits.h"

class dtNavMeshQuery;
class dtNavMesh;

namespace LevEngine
{
    REGISTER_PARSE_TYPE(AIAgentComponent);
	
    struct AIAgentComponent
    {
        AIAgentComponent();
        AIAgentComponent(const AIAgentComponent&) = default;

        bool isActive = false; 
        
        dtCrowdAgentParams* GetAgentParams() const;
        void SetAgentParams(dtCrowdAgentParams* params);
        
    private:
        
        dtCrowdAgent* m_agent;
        dtCrowdAgentParams* m_agentParams;
    };
}
