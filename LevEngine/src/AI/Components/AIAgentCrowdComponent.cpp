#include "levpch.h"
#include "AIAgentCrowdComponent.h"

#include <DetourCrowd.h>

#include "AIAgentComponent.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Scene/Components/NavMesh/NavMeshComponent.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    AIAgentCrowdComponent::AIAgentCrowdComponent() : m_crowd(0)
    {
        m_crowd = dtAllocCrowd();
        m_crowdAgentDebugInfo = new dtCrowdAgentDebugInfo();
    }

    void AIAgentCrowdComponent::Init()
    {
        if(!navMesh)
        {
            Log::CoreError("AI: Cannot initialize crowd without NavMesh");
            m_isInitializationFailed = true;
            return;
        }
        const auto& navMeshComponent = navMesh.GetComponent<NavMeshComponent>();
        if(!navMeshComponent.IsBuilded)
        {
            Log::CoreError("AI: Cannot initialize crowd when NavMesh isn't builded");
            m_isInitializationFailed = true;
            return;
        }
        m_navMesh = navMeshComponent.GetNavMesh();
        m_navMeshQuery = navMeshComponent.GetNavMeshQuery();
        m_crowd->init(25, navMeshComponent.AgentRadius, navMeshComponent.GetNavMesh());
        RegisterDefaultObstacleAvoidanceProfiles();
        for (auto agentEntity : agentsEntities)
        {
            AddAgent(agentEntity);
        }
        m_isInitialized = true;

        SetMoveTarget(0, Vector3(-12.652f, 0.000f, 13.043f));
    }

    void AIAgentCrowdComponent::RegisterDefaultObstacleAvoidanceProfiles()
    {
        dtObstacleAvoidanceParams params;
        
        params.weightDesVel = 2.0f;
        params.weightCurVel = 0.75f;
        params.weightSide = 0.75f;
        params.weightToi = 2.5f;
        params.horizTime = 2.5f;
        params.gridSize = 33;
        
        params.velBias = 0.5f;
        params.adaptiveDivs = 5;
        params.adaptiveRings = 2;
        params.adaptiveDepth = 1;
        m_crowd->setObstacleAvoidanceParams(static_cast<int>(ObstacleAvoidanceQuality::Low), &params);
        
        params.velBias = 0.5f;
        params.adaptiveDivs = 5; 
        params.adaptiveRings = 2;
        params.adaptiveDepth = 2;
        m_crowd->setObstacleAvoidanceParams(static_cast<int>(ObstacleAvoidanceQuality::Medium), &params);
        
        params.velBias = 0.5f;
        params.adaptiveDivs = 7;
        params.adaptiveRings = 2;
        params.adaptiveDepth = 3;
        m_crowd->setObstacleAvoidanceParams(static_cast<int>(ObstacleAvoidanceQuality::High), &params);
        
        params.velBias = 0.5f;
        params.adaptiveDivs = 7;
        params.adaptiveRings = 3;
        params.adaptiveDepth = 3;
        m_crowd->setObstacleAvoidanceParams(static_cast<int>(ObstacleAvoidanceQuality::Ultra), &params);
    }

    void AIAgentCrowdComponent::Update(const float deltaTime)
    {
        if(!m_isInitialized)
        {
            return;
        }
        m_crowd->update(deltaTime, m_crowdAgentDebugInfo);
    }

    void AIAgentCrowdComponent::UpdateAgentsPosition()
    {
        for(int i = 0; i < agentsEntities.size(); ++i)
        {
            const auto agent = m_crowd->getAgent(i);
            if(!agent)
            {
                continue;
            }
            auto& agentTransform = agentsEntities[i].GetComponent<Transform>();
            const Vector3 currentPosition = {agent->npos[0], agent->npos[1], agent->npos[2]};
            agentTransform.SetWorldPosition(currentPosition);
        }
    }

    bool AIAgentCrowdComponent::IsInitialized() const
    {
        return m_isInitialized;
    }
    
    bool AIAgentCrowdComponent::IsInitializationFailed() const
    {
        return m_isInitializationFailed;
    }

    void AIAgentCrowdComponent::AddAgent(const Entity& agentEntity)
    {
        const auto& agentComponent = agentEntity.GetComponent<AIAgentComponent>();
        const auto& agentTransform = agentEntity.GetComponent<Transform>();

        const Vector3 pos = agentTransform.GetLocalPosition();
            
        m_crowd->addAgent(&pos.x, agentComponent.GetAgentParams());
    }

    void AIAgentCrowdComponent::SetMoveTarget(int agentIndex, Vector3 targetPos)
    {
        const auto agent = m_crowd->getAgent(agentIndex);
        if (!agent || !agent->active)
        {
            return;
        }
        
        const float* halfExtents = m_crowd->getQueryHalfExtents();
        const dtQueryFilter* filter = m_crowd->getFilter(0);

        float m_targetPos[3];
        m_navMeshQuery->findNearestPoly(&targetPos.x, halfExtents, filter, &m_targetRef, m_targetPos);
        m_crowd->requestMoveTarget(agentIndex, m_targetRef, m_targetPos);
    }

    class AIAgentCrowdComponentSerializer final : public ComponentSerializer<AIAgentCrowdComponent, AIAgentCrowdComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "AIAgentCrowd"; }
        
        void SerializeData(YAML::Emitter& out, const AIAgentCrowdComponent& component) override
        {
            out << YAML::Key << "Agents count" << YAML::Value << static_cast<int>(component.agentsEntities.size());
            out << YAML::Key << "Agents" << YAML::Value << YAML::BeginSeq;
            for (auto agent : component.agentsEntities)
            {
                out << YAML::BeginMap;
            
                if (agent)
                {
                    out << YAML::Key << "Agent" << YAML::Value << agent.GetUUID();
                }
                else
                {
                    out << YAML::Key << "Agent" << YAML::Value << YAML::Null;
                }
                
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;
            if(component.navMesh)
            {
                out << YAML::Key << "Navigation mesh" << YAML::Value << component.navMesh.GetUUID();
            }
        }
        
        void DeserializeData(const YAML::Node& node, AIAgentCrowdComponent& component) override
        {
            const int componentCount = node["Agents count"].as<int>();
            component.agentsEntities = Vector<Entity>(componentCount);
            
            const auto agentsNodes = node["Agents"];
            if (!agentsNodes)
                return;
            
            int componentIdx = 0;
            for (auto agent : agentsNodes)
            {
                auto agentNode = agent["Agent"];
                
                component.agentsEntities[componentIdx] = !agentNode.IsNull()
                    ? SceneManager::GetActiveScene()->GetEntityByUUID(UUID(agentNode.as<uint64_t>()))
                    : Entity();
                
                componentIdx++;
            }
            if(auto navMeshNode = node["Navigation mesh"])
            {
                component.navMesh = !navMeshNode.IsNull()
                    ? SceneManager::GetActiveScene()->GetEntityByUUID(UUID(navMeshNode.as<uint64_t>()))
                    : Entity();
            }
        }
    };
}
