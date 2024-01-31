#include "levpch.h"
#include "AIAgentComponent.h"

#include "AIAgentCrowdComponent.h"
#include "Scene/Components/ComponentSerializer.h"

#include "Rule.h"

#include <regex>

#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/Components/CharacterController.h"
#include "Physics/Components/Rigidbody.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/Transform/Transform.h"

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

	Vector<Entity> AIAgentComponent::FindEntitiesInVisibleScope(FilterLayer layerMask) const
    {
    	if(!m_initialized || !VisionCollider)
    	{
    		return {};
    	}
    	
    	const auto& agentTransform = m_selfEntity.GetComponent<Transform>();
    	const auto& agentCharacterController = m_selfEntity.GetComponent<CharacterController>();
    	
    	const auto& agentPosition = agentTransform.GetWorldPosition();
    	const auto& centerOffset = agentCharacterController.GetCenterOffset();
    	const auto& capsuleHalfHeight = agentCharacterController.GetHeight();
    	Vector3 eyePoint = agentPosition + centerOffset;
    	eyePoint.y += capsuleHalfHeight;

    	const auto& rigidBodyComponent = VisionCollider.GetComponent<Rigidbody>();
    	
    	const auto& entitiesInCollider = rigidBodyComponent.GetTriggerStayBuffer();

    	Vector<Entity> filteredEntities;
    	for (auto entity : entitiesInCollider)
    	{
    		if(!entity)
    		{
    			continue;
    		}
    		if(entity.HasComponent<Rigidbody>())
    		{
    			const auto& rigidBody = entity.GetComponent<Rigidbody>();
    			const auto layer = rigidBody.GetLayer();
    			if (layer & layerMask)
    			{
    				filteredEntities.push_back(entity);
    			}
    		}
		    else if (entity.HasComponent<CharacterController>())
		    {
		    	const auto& characterController = entity.GetComponent<CharacterController>();
			    const auto layer = characterController.GetLayer();
		    	if (layer & layerMask)
		    	{
		    		filteredEntities.push_back(entity);
		    	}
		    }
    	}

    	Vector<Entity> findedEntities;
	    for (auto filteredEntity : filteredEntities)
	    {
	    	const auto& entityTransform = filteredEntity.GetComponent<Transform>();
	    	const auto& forwardDirection = agentTransform.GetForwardDirection();
	    	const auto& targetTransform = entityTransform.GetWorldPosition();

	    	Vector3 fromAgentToTarget = (targetTransform - eyePoint);
	    	fromAgentToTarget.Normalize();
    	
	    	float dotProduct = forwardDirection.Dot(fromAgentToTarget);	
	    	dotProduct /= (forwardDirection.Length() * fromAgentToTarget.Length());
	    	const float angleBetweenVisionVectorAndTarget = acos(dotProduct) * 180.0 / 3.14159265;
	    	
	    	if(angleBetweenVisionVectorAndTarget > AngleOfVision)
	    	{
	    		continue;
	    	}
    	
	    	const auto& rayCastHitResult = Application::Get().GetPhysics().Raycast(eyePoint, fromAgentToTarget,
				RangeOfVision, layerMask);
    	
	    	if(!rayCastHitResult.IsSuccessful)
	    	{
	    		continue;
	    	}
    	
	    	findedEntities.push_back(filteredEntity);
	    }

    	if(findedEntities.size() == 0)
    	{
    		return {};
    	}

    	return findedEntities;
    }

	Entity AIAgentComponent::FindClosestEntityInVisibleScope(FilterLayer layerMask) const
    {
    	if(!m_selfEntity)
    	{
    		return {};
    	}
    	if(!m_selfEntity.HasComponent<Transform>())
		{
			return {};
		}
    	const auto& selfTransform = m_selfEntity.GetComponent<Transform>();
    	float minDistance = std::numeric_limits<float>::max();
    	Entity closestEntity;
    	Vector<Entity> entities = FindEntitiesInVisibleScope(layerMask);
	    for (auto entity : entities)
	    {
	    	if(!entity)
	    	{
	    		continue;
	    	}
	    	const auto& entityTransform = entity.GetComponent<Transform>();
		    const float distance = Vector3::DistanceSquared(selfTransform.GetWorldPosition(), entityTransform.GetWorldPosition());
	    	if (distance < minDistance)
	    	{
	    		minDistance = distance;
	    		closestEntity = entity;
	    	}
	    }

    	if(closestEntity)
    	{
    		return closestEntity;
    	}

    	return {};
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
    	if(!m_isRBSDirty)
    	{
    		return m_lastMatchedRule;
    	}
		String bestRule;
    	int priorityOfBestRule = -1;
		for (auto& rule : m_rules)
		{
			bool matched = true;
			if(rule.GetPriority() < priorityOfBestRule)
			{
				matched = false;
			}
			else
			{
				for (const auto& condition : rule.GetConditions())
				{
					bool conditionPassed = false;
					const auto& conditionId = condition.id;
					const auto& conditionAttribute = condition.attribute;
					const auto& conditionOperation = condition.operation;
					const auto& idAndAttribute = MakePair(conditionId, conditionAttribute);
					switch (condition.type)
					{
						case RuleConditionType::Bool:
							{
								if (HasBoolFact(idAndAttribute))
								{
									const auto& factValue = GetFactAsBool(idAndAttribute);
									
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
								if (HasNumberFact(idAndAttribute))
								{
									const auto& factValue = GetFactAsNumber(MakePair(conditionId, conditionAttribute));
				
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
								if (HasVector3Fact(idAndAttribute))
								{
									const auto& factValue = GetFactAsVector3(idAndAttribute);
									
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
								if (HasStringFact(idAndAttribute))
								{
									const auto& factValue = GetFactAsString(idAndAttribute);
				
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
					case RuleConditionType::Entity:
							{
								if (HasEntityFact(idAndAttribute))
								{
									const auto& factValue = GetFactAsEntity(idAndAttribute);
									
									if(conditionOperation == "==" && factValue == condition.entityValue)
									{
										conditionPassed = true;
										break;
									}
									if(conditionOperation == "~=" && factValue != condition.entityValue)
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
			}

			if(matched)
			{
				bestRule = rule.GetName();
				priorityOfBestRule = rule.GetPriority();
			}
		}

    	if(priorityOfBestRule != -1)
    	{
    		m_isRBSDirty = false;
    		m_lastMatchedRule = bestRule;
    		return m_lastMatchedRule;
    	}
    	
    	return {};
	}

    dtCrowdAgentParams* AIAgentComponent::GetAgentParams() const
    {
    	return m_agentParams;
    }

    void AIAgentComponent::SetAgentParams(dtCrowdAgentParams* params)
    {
    	m_agentParams = params;
    }

	void AIAgentComponent::SetMoveTarget(Entity entity)
	{
		if(m_initialized)
		{
			m_targetEntity = entity;
			HasTargetEntity = true;
		}
	}

	Entity AIAgentComponent::GetMoveTarget() const
	{
	    return m_targetEntity;
    }

    void AIAgentComponent::SetMovePoint(Vector3 targetPos)
    {
    	if(m_initialized)
    	{
    		auto& crowdComponent = m_crowd.GetComponent<AIAgentCrowdComponent>();
    		crowdComponent.SetMoveTarget(m_agentIndex, targetPos);
    		HasTargetEntity = false;
    	}
    }
	
	void AIAgentComponent::SetFactAsBool(const Pair<String, String>& key, bool value)
    {
    	const auto it = m_boolFacts.find(key);

    	if(it == m_boolFacts.end() || it->second != value)
    	{
    		m_isRBSDirty = true;
    	}

    	m_boolFacts[key] = value;
    }

	bool AIAgentComponent::HasBoolFact(const Pair<String, String>& key)
    {
	    for (auto keyValue : m_boolFacts)
	    {
	    	std::regex self_regex(key.first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::icase);
	    	if (std::regex_search(keyValue.first.first.c_str(), self_regex))
	    	{
	    		return true;
	    	}
	    }
    	return false;
    }

	bool AIAgentComponent::GetFactAsBool(const Pair<String, String>& key)
    {
    	for (auto keyValue : m_boolFacts)
    	{
    		std::regex self_regex(key.first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::icase);
    		if (std::regex_search(keyValue.first.first.c_str(), self_regex))
    		{
    			return keyValue.second;
    		}
    	}
    	return {};
    }

	void AIAgentComponent::SetFactAsNumber(const Pair<String, String>& key, float value)
	{
    	const auto it = m_numberFacts.find(key);

    	if(it == m_numberFacts.end() || it->second != value)
    	{
    		m_isRBSDirty = true;
    	}

    	m_numberFacts[key] = value;
	}

	bool AIAgentComponent::HasNumberFact(const Pair<String, String>& key)
	{
    	for (auto keyValue : m_numberFacts)
    	{
    		std::regex self_regex(key.first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::icase);
    		if (std::regex_search(keyValue.first.first.c_str(), self_regex))
    		{
    			return true;
    		}
    	}
    	return false;
	}

	float AIAgentComponent::GetFactAsNumber(const Pair<String, String>& key)
	{
    	for (auto keyValue : m_numberFacts)
    	{
    		std::regex self_regex(key.first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::icase);
    		if (std::regex_search(keyValue.first.first.c_str(), self_regex))
    		{
    			return keyValue.second;
    		}
    	}
    	return {};
	}

	void AIAgentComponent::SetFactAsVector3(const Pair<String, String>& key, Vector3 value)
    {
    	const auto it = m_vector3Facts.find(key);

    	if(it == m_vector3Facts.end() || it->second != value)
    	{
    		m_isRBSDirty = true;
    	}
    	
    	m_vector3Facts[key] = value;
    }

	bool AIAgentComponent::HasVector3Fact(const Pair<String, String>& key)
	{
    	for (auto keyValue : m_vector3Facts)
    	{
    		std::regex self_regex(key.first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::icase);
    		if (std::regex_search(keyValue.first.first.c_str(), self_regex))
    		{
    			return true;
    		}
    	}
    	return false;
	}

	Vector3 AIAgentComponent::GetFactAsVector3(const Pair<String, String>& key)
    {
    	for (auto keyValue : m_vector3Facts)
    	{
    		std::regex self_regex(key.first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::icase);
    		if (std::regex_search(keyValue.first.first.c_str(), self_regex))
    		{
    			return keyValue.second;
    		}
    	}
    	return {};
    }

	void AIAgentComponent::SetFactAsString(const Pair<String, String>& key, const String& value)
	{
    	const auto it = m_stringFacts.find(key);

    	if(it == m_stringFacts.end() || it->second != value)
    	{
    		m_isRBSDirty = true;
    	}
    	
    	m_stringFacts[key] = value;
	}

	bool AIAgentComponent::HasStringFact(const Pair<String, String>& key)
	{
    	for (auto keyValue : m_stringFacts)
    	{
    		std::regex self_regex(key.first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::icase);
    		if (std::regex_search(keyValue.first.first.c_str(), self_regex))
    		{
    			return true;
    		}
    	}
    	return false;
	}

	String AIAgentComponent::GetFactAsString(const Pair<String, String>& key)
	{
    	for (auto keyValue : m_stringFacts)
    	{
    		std::regex self_regex(key.first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::icase);
    		if (std::regex_search(keyValue.first.first.c_str(), self_regex))
    		{
    			return keyValue.second;
    		}
    	}
    	return {};
	}

	void AIAgentComponent::SetFactAsEntity(const Pair<String, String>& key, Entity value)
	{
    	const auto it = m_entityFacts.find(key);

    	if(it == m_entityFacts.end() || it->second != value)
    	{
    		m_isRBSDirty = true;
    	}
    	
    	m_entityFacts[key] = value;
	}

	bool AIAgentComponent::HasEntityFact(const Pair<String, String>& key)
	{
    	for (auto keyValue : m_entityFacts)
    	{
    		std::regex self_regex(key.first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::icase);
    		if (std::regex_search(keyValue.first.first.c_str(), self_regex))
    		{
    			return true;
    		}
    	}
    	return false;
	}

	Entity AIAgentComponent::GetFactAsEntity(const Pair<String, String>& key)
	{
    	for (auto keyValue : m_entityFacts)
    	{
    		std::regex self_regex(key.first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::icase);
    		if (std::regex_search(keyValue.first.first.c_str(), self_regex))
    		{
    			return keyValue.second;
    		}
    	}
    	return {};
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
        	if(component.VisionCollider)
        	{
        		out << YAML::Key << "Vision collider" << YAML::Value << component.VisionCollider.GetUUID();
        	}
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
        	if(const auto visionColliderNode = node["Vision collider"])
        	{
        		component.VisionCollider = !visionColliderNode.IsNull()
					? SceneManager::GetActiveScene()->GetEntityByUUID(UUID(visionColliderNode.as<uint64_t>()))
					: Entity();
        	}
        }
    };
}

