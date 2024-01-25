#include "levpch.h"
#include "AIAgentComponent.h"

#include "AIAgentCrowdComponent.h"
#include "Scene/Components/ComponentSerializer.h"

#include "Rule.h"

namespace LevEngine
{
    AIAgentComponent::AIAgentComponent()
    {
	    m_agentParams = new dtCrowdAgentParams();
    	
    	m_agentParams->radius = 0.6f;
    	m_agentParams->height = 2.0f;
    	m_agentParams->maxAcceleration = 8.0f;
    	m_agentParams->maxSpeed = 3.5f;
    	m_agentParams->collisionQueryRange = m_agentParams->radius * 12.0f;
    	m_agentParams->pathOptimizationRange = m_agentParams->radius * 30.0f;
    	m_agentParams->separationWeight = 2.0f;
    	m_agentParams->updateFlags = 0;
    	m_agentParams->updateFlags |= DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_OBSTACLE_AVOIDANCE |
			DT_CROWD_OPTIMIZE_VIS |DT_CROWD_OPTIMIZE_TOPO;
    	m_agentParams->obstacleAvoidanceType = static_cast<int>(ObstacleAvoidanceQuality::Ultra);
    	m_agentParams->queryFilterType = 0;

    	m_agent = new dtCrowdAgent();

    	m_agent->active = false;
    	m_agent->params = *m_agentParams;
    }

    void AIAgentComponent::Init(Entity crowd, Entity agent, int agentIndex)
    {
    	m_crowd = crowd;
    	m_selfEntity = agent;
    	m_agentIndex = agentIndex;

    	m_initialized = true;
    }

	void AIAgentComponent::OnDestroy(const Entity entity)
    {
    	AIAgentComponent& component = entity.GetComponent<AIAgentComponent>();
    	component.OnComponentDestroy();
    }

	void AIAgentComponent::OnComponentDestroy()
    {
    	if (!m_crowd) return;

    	if(m_crowd.HasComponent<AIAgentCrowdComponent>())
    	{
    		auto& crowdComponent = m_crowd.GetComponent<AIAgentCrowdComponent>();
    		crowdComponent.RemoveAgent(m_selfEntity);
    	}
    }

	Entity AIAgentComponent::GetCrowd() const
    {
	    return m_crowd;
    }
	
	int AIAgentComponent::GetIndexInCrowd() const
	{
	    return m_agentIndex;
    }

	void AIAgentComponent::AddRule(const Rule& newRule)
    {
	    m_rules.push_back(newRule);
    }

	void AIAgentComponent::InitRBS()
	{
    	m_RBSInited = true;
	}

	bool AIAgentComponent::IsRBSInited()
	{
    	return m_RBSInited;
	}

	const String& AIAgentComponent::Match()
	{
		for (auto& rule : m_rules)
		{
			bool matched = true;
			for (const auto& condition : rule.GetConditions())
			{
				bool conditionPassed = false;
				const auto& conditionName = condition.name;
				const auto& conditionOperation = condition.operation;
				switch (condition.type)
				{
					case RuleConditionType::Bool:
						{
							if (HasBoolFact(conditionName))
							{
								const auto& factValue = GetFactAsBool(conditionName);
								
								if(conditionOperation == "==" && factValue == condition.boolValue)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == "~=" && factValue != condition.boolValue)
								{
									conditionPassed = true;
									break;
								}
							}
							break;
						}
					case RuleConditionType::Number:
						{
							if (HasNumberFact(conditionName))
							{
								const auto& factValue = GetFactAsNumber(conditionName);
			
								if(conditionOperation == "==" && factValue == condition.numberValue)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == "~=" && factValue != condition.numberValue)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == "<" && factValue < condition.numberValue)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == ">" && factValue > condition.numberValue)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == "<=" && factValue <= condition.numberValue)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == ">=" && factValue >= condition.numberValue)
								{
									conditionPassed = true;
									break;
								}
							}
							break;
						}
					case RuleConditionType::Vector3:
						{
							if (HasVector3Fact(conditionName))
							{
								const auto& factValue = GetFactAsVector3(conditionName);
								
								if(conditionOperation == "==" && factValue == condition.vector3Value)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == "~=" && factValue != condition.vector3Value)
								{
									conditionPassed = true;
									break;
								}
							}
							break;
						}
					case RuleConditionType::String:
						{
							if (HasStringFact(conditionName))
							{
								const auto& factValue = GetFactAsString(conditionName);
			
								if(conditionOperation == "==" && factValue == condition.stringValue)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == "~=" && factValue != condition.stringValue)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == "<" && factValue < condition.stringValue)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == ">" && factValue > condition.stringValue)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == "<=" && factValue <= condition.stringValue)
								{
									conditionPassed = true;
									break;
								}
								if(conditionOperation == ">=" && factValue >= condition.stringValue)
								{
									conditionPassed = true;
									break;
								}
							}
							break;
						}
				}
			
				if (!conditionPassed)
				{
					matched = false;
				}
			}

			if(matched)
			{
				return rule.GetName();
			}
		}
    	
    	return nullptr;
	}

    dtCrowdAgentParams* AIAgentComponent::GetAgentParams() const
    {
    	return m_agentParams;
    }

    void AIAgentComponent::SetAgentParams(dtCrowdAgentParams* params)
    {
    	m_agentParams = params;
    }

    void AIAgentComponent::SetMoveTarget(Vector3 targetPos)
    {
    	if(m_initialized)
    	{
    		auto& crowdComponent = m_crowd.GetComponent<AIAgentCrowdComponent>();
    		crowdComponent.SetMoveTarget(m_agentIndex, targetPos);
    	}
    }
	
	void AIAgentComponent::SetFactAsBool(const String& key, bool value)
    {
    	m_boolFacts[key] = value;
    }

	bool AIAgentComponent::HasBoolFact(const String& key)
    {
    	const auto it = m_boolFacts.find(key);

    	if(it == m_boolFacts.end())
    	{
    		return false;
    	}

    	return true;
    }

	bool AIAgentComponent::GetFactAsBool(const String& key)
    {
	    const auto it = m_boolFacts.find(key);

    	if(it == m_boolFacts.end())
    	{
    		return {};
    	}

    	return m_boolFacts[key];
    }

	void AIAgentComponent::SetFactAsNumber(const String& key, float value)
	{
    	m_numberFacts[key] = value;
	}

	bool AIAgentComponent::HasNumberFact(const String& key)
	{
    	const auto it = m_numberFacts.find(key);

    	if(it == m_numberFacts.end())
    	{
    		return false;
    	}

    	return true;
	}

	float AIAgentComponent::GetFactAsNumber(const String& key)
	{
    	const auto it = m_numberFacts.find(key);

    	if(it == m_numberFacts.end())
    	{
    		return {};
    	}

    	return m_numberFacts[key];
	}

	void AIAgentComponent::SetFactAsVector3(const String& key, Vector3 value)
    {
    	m_vector3Facts[key] = value;
    }

	bool AIAgentComponent::HasVector3Fact(const String& key)
	{
    	const auto it = m_vector3Facts.find(key);

    	if(it == m_vector3Facts.end())
    	{
    		return false;
    	}

    	return true;
	}

	Vector3 AIAgentComponent::GetFactAsVector3(const String& key)
    {
    	const auto it = m_vector3Facts.find(key);

    	if(it == m_vector3Facts.end())
    	{
    		return {};
    	}
    	
    	return m_vector3Facts[key];
    }

	void AIAgentComponent::SetFactAsString(const String& key, const String& value)
	{
    	m_stringFacts[key] = value;
	}

	bool AIAgentComponent::HasStringFact(const String& key)
	{
    	const auto it = m_stringFacts.find(key);

    	if(it == m_stringFacts.end())
    	{
    		return false;
    	}

    	return true;
	}

	String AIAgentComponent::GetFactAsString(const String& key)
	{
    	const auto it = m_stringFacts.find(key);

    	if(it == m_stringFacts.end())
    	{
    		return {};
    	}
    	
    	return m_stringFacts[key];
	}

	class AIAgentComponentSerializer final : public ComponentSerializer<AIAgentComponent, AIAgentComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "AIAgentComponent"; }
        
        void SerializeData(YAML::Emitter& out, const AIAgentComponent& component) override
        {
            out << YAML::Key << "Is active" << YAML::Value << component.isActive;
        	const auto agentParams = component.GetAgentParams();
			out << YAML::Key << "Agent radius" << YAML::Value << agentParams->radius;
        	out << YAML::Key << "Agent height" << YAML::Value << agentParams->height;
        	out << YAML::Key << "Max acceleration" << YAML::Value << agentParams->maxAcceleration;
        	out << YAML::Key << "Max speed" << YAML::Value << agentParams->maxSpeed;
        	out << YAML::Key << "Collision query range" << YAML::Value << agentParams->collisionQueryRange;
        	out << YAML::Key << "Path optimization range" << YAML::Value << agentParams->pathOptimizationRange;
        	out << YAML::Key << "Separation weight" << YAML::Value << agentParams->separationWeight;
        	out << YAML::Key << "Update flags" << YAML::Value << static_cast<int>(agentParams->updateFlags);
        	out << YAML::Key << "Obstacle avoidance type" << YAML::Value << static_cast<int>(agentParams->obstacleAvoidanceType);
        	out << YAML::Key << "Query filter type" << YAML::Value << static_cast<int>(agentParams->queryFilterType);
        }
        
        void DeserializeData(const YAML::Node& node, AIAgentComponent& component) override
        {
        	component.isActive = node["Is active"].as<bool>();
        	const auto agentParams = component.GetAgentParams();
        	agentParams->radius = node["Agent radius"].as<float>();
        	agentParams->height = node["Agent height"].as<float>();
        	agentParams->maxAcceleration = node["Max acceleration"].as<float>();
        	agentParams->maxSpeed = node["Max speed"].as<float>();
        	agentParams->collisionQueryRange = node["Collision query range"].as<float>();
        	agentParams->pathOptimizationRange = node["Path optimization range"].as<float>();
        	agentParams->separationWeight = node["Separation weight"].as<float>();
        	agentParams->updateFlags = node["Update flags"].as<int>();
        	agentParams->obstacleAvoidanceType = node["Obstacle avoidance type"].as<int>();
        	agentParams->queryFilterType = node["Query filter type"].as<int>();
        }
    };
}

