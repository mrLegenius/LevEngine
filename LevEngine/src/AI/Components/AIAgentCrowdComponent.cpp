#include "levpch.h"
#include "AIAgentCrowdComponent.h"

#include <DetourCrowd.h>

#include "AIAgentComponent.h"
#include "Math/Random.h"
#include "Physics/Components/CharacterController.h"
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

    void AIAgentCrowdComponent::Init(Entity crowdEntity)
    {
        m_selfEntity = crowdEntity;
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
        m_crowd->init(MAX_AGENTS_COUNT, navMeshComponent.AgentRadius, navMeshComponent.GetNavMesh());
        RegisterDefaultObstacleAvoidanceProfiles();
        for (Entity agentEntity : initialAgentsEntities)
        {
            if(agentEntity)
            {
                AddAgent(agentEntity);
            }
        }
        m_isInitialized = true;
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

    void AIAgentCrowdComponent::Update(float deltaTime)
    {
        if(!m_isInitialized)
        {
            return;
        }
        
        for (const auto& agentEntity : agents)
        {
            auto& agentComponent = agentEntity.GetComponent<AIAgentComponent>();
            const int index = agentComponent.GetIndexInCrowd();
            const auto agent = m_crowd->getEditableAgent(index);
            if(!agent || !agent->active)
            {
                continue;
            }
            auto& agentTransform = agentEntity.GetComponent<Transform>();
            const Vector3& position = agentTransform.GetWorldPosition();
            agent->npos[0] = position.x;
            agent->npos[1] = position.y;
            agent->npos[2] = position.z;

            if(agentComponent.HasTargetEntity)
            {
                if(Entity targetEntity = agentComponent.GetMoveTarget())
                {
                    if(targetEntity.HasComponent<Transform>())
                    {
                        const auto& entityTransform = targetEntity.GetComponent<Transform>();
                        SetMoveTarget(index, entityTransform.GetWorldPosition());
                    }
                }
            }
        }
        
        m_crowd->update(deltaTime, m_crowdAgentDebugInfo);
    }

    void AIAgentCrowdComponent::UpdateAgentsPosition()
    {
        for (const auto& agentEntity : agents)
        {
            auto& agentComponent = agentEntity.GetComponent<AIAgentComponent>();
            const int index = agentComponent.GetIndexInCrowd();
            const auto agent = m_crowd->getAgent(index);
            if(!agent)
            {
                continue;
            }
            if(!agentEntity.HasComponent<CharacterController>())
            {
                continue;
            }
            auto& characterController = agentEntity.GetComponent<CharacterController>();
            auto & characterTransform = agentEntity.GetComponent<Transform>();
            Vector3 velocity = {agent->nvel[0], agent->nvel[1], agent->nvel[2]};

            velocity.y = 0.0f;
            velocity.Normalize();
            
            characterTransform.SetWorldRotation(Quaternion::LookRotation(velocity, Vector3::Up));
            
            const Vector3 currentPosition = {agent->npos[0], agent->npos[1], agent->npos[2]};
            characterController.MoveTo(currentPosition);
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

    void AIAgentCrowdComponent::AddAgent(Entity agentEntity)
    {
        agents.push_back(agentEntity); 

        const auto& agentTransform = agentEntity.GetComponent<Transform>();
        auto& agentComponent = agentEntity.GetComponent<AIAgentComponent>();
        
        const Vector3 pos = agentTransform.GetLocalPosition();

        const int agentIndex = m_crowd->addAgent(&pos.x, agentComponent.GetAgentParams());
        
        agentComponent.Init(m_selfEntity, agentEntity, agentIndex);
    }

    void AIAgentCrowdComponent::RemoveAgent(Entity agentEntity)
    {
        const auto it = eastl::find(agents.begin(), agents.end(), agentEntity);

        if(it == agents.end())
        {
            return;
        }
        
        const int index = eastl::distance(agents.begin(), it);
        
        agents.erase(it);
        m_crowd->removeAgent(index);
    }

    Vector3 AIAgentCrowdComponent::GetRandomPointOnNavMesh() const
    {
        if(!IsInitialized())
        {
            return Vector3::Zero;
        }

        const dtQueryFilter* filter = m_crowd->getFilter(0);
        
        float point[3];
        dtPolyRef polyRef;
        
        m_navMeshQuery->findRandomPoint(filter, Random::Float, &polyRef, point);

        return {point[0], point[1], point[2]};
    }

    Vector3 AIAgentCrowdComponent::GetRandomPointAroundCircle(const Vector3& searchCircleCenter,
                                                                     float searchCircleRadius) const
    {
        if(!IsInitialized())
        {
            return Vector3::Zero;
        }

        const float* center = &searchCircleCenter.x;
        const float radius = searchCircleRadius;

        const dtQueryFilter* filter = m_crowd->getFilter(0);
        
        float point[3];
        dtPolyRef startPolyRef;

        const float halfExtents[3] = {radius/2, radius/2, radius/2};

        m_navMeshQuery->findNearestPoly(center, halfExtents, filter, &startPolyRef, point);

        dtPolyRef resultPolyRef;
        
        m_navMeshQuery->findRandomPointAroundCircle(startPolyRef, center, radius, filter, Random::Float, &resultPolyRef, point);

        return {point[0], point[1], point[2]};
    }

    Vector3 AIAgentCrowdComponent::GetNearestPoint(const Vector3& searchBoxCenter,
        const Vector3& searchBoxHalfExtents) const
    {
        if(!IsInitialized())
        {
            return Vector3::Zero;
        }

        const float* center = &searchBoxCenter.x;
        const float* halfExtents = &searchBoxHalfExtents.x;

        const dtQueryFilter* filter = m_crowd->getFilter(0);
        
        float point[3];
        dtPolyRef polyRef;
        
        m_navMeshQuery->findNearestPoly(center, halfExtents, filter, &polyRef, point);

        return {point[0], point[1], point[2]};
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
            out << YAML::Key << "Agents count" << YAML::Value << static_cast<int>(component.initialAgentsEntities.size());
            out << YAML::Key << "Agents" << YAML::Value << YAML::BeginSeq;
            for (auto agent : component.initialAgentsEntities)
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
            component.initialAgentsEntities = Vector<Entity>(componentCount);
            
            const auto agentsNodes = node["Agents"];
            if (!agentsNodes)
                return;
            
            int componentIdx = 0;
            for (auto agent : agentsNodes)
            {
                auto agentNode = agent["Agent"];
                
                component.initialAgentsEntities[componentIdx] = !agentNode.IsNull()
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
