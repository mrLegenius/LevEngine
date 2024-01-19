#include "levpch.h"
#include "AIAgentCrowdUpdateSystem.h"

#include "AI/Components/AIAgentCrowdComponent.h"

namespace LevEngine
{
    void AIAgentCrowdUpdateSystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<AIAgentCrowdComponent>();

        for (const auto entity : view)
        {
            auto& agentCrowdComponent = view.get<AIAgentCrowdComponent>(entity);

            if(agentCrowdComponent.IsInitializationFailed())
            {
                continue;
            }
            
            if(!agentCrowdComponent.IsInitialized())
            {
                const auto entityWrapped = Entity(entt::handle{ registry, entity });
                agentCrowdComponent.Init(entityWrapped);    
            }
            
            agentCrowdComponent.Update(deltaTime);
            agentCrowdComponent.UpdateAgentsPosition(deltaTime);
        }
    }
}
