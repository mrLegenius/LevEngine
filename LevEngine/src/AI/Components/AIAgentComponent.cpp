#include "levpch.h"
#include "AIAgentComponent.h"

#include "AIAgentCrowdComponent.h"
#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/Components/CharacterController.h"
#include "Physics/Components/Rigidbody.h"
#include "Scene/Components/ComponentSerializer.h"
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

	void AIAgentComponent::OnDestroy(entt::registry& registry, entt::entity entity)
    {
    	AIAgentComponent& component = registry.get<AIAgentComponent>(entity);
    	component.OnComponentDestroy();
    }

	void AIAgentComponent::OnComponentDestroy()
    {
    	if(m_crowd.HasComponent<AIAgentCrowdComponent>())
    	{
    		auto& crowdComponent = m_crowd.GetComponent<AIAgentCrowdComponent>();
    		crowdComponent.RemoveAgent(m_selfEntity);	
    	}
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

	Entity AIAgentComponent::FindEntityInVisibleScope(int layer) const
    {
    	if(!m_initialized || !visionCollider)
    	{
    		return {};
    	}
    	
    	const auto& agentTransform = m_selfEntity.GetComponent<Transform>();
    	const auto& characterController = m_selfEntity.GetComponent<CharacterController>();
    	
    	const auto& agentPosition = agentTransform.GetWorldPosition();
    	const auto& centerOffset = characterController.GetCenterOffset();
    	const auto& capsuleHalfHeight = characterController.GetHeight();
    	Vector3 eyePoint = agentPosition + centerOffset;
    	eyePoint.y += capsuleHalfHeight;

    	const auto& rigidBodyComponent = visionCollider.GetComponent<Rigidbody>();
    	
    	const auto& findedEntities = rigidBodyComponent.GetTriggerEnterBuffer();

    	if(findedEntities.size() > 0)
    	{
    		Log::CoreInfo(findedEntities.size());
    	}

    	const auto& forwardDirection = agentTransform.GetForwardDirection();

    	return {};

    	// const Vector3 fromAgentToTarget = sphereCastHitResult.Point - agentPosition;
    	//
    	// float dotProduct = forwardDirection.Dot(fromAgentToTarget);
    	// dotProduct /= (forwardDirection.Length() * fromAgentToTarget.Length());
    	// const float angleBetweenVisionVectorAndTarget = acos(dotProduct);
    	//
    	// if(angleBetweenVisionVectorAndTarget > AngleOfVision)
    	// {
    	// 	return {};
    	// }
	    //
    	// const auto& rayCastHitResult = Application::Get().GetPhysics().Raycast(eyePoint, forwardDirection,
    	// 	RangeOfVision, static_cast<FilterLayer>(layer));
	    //
    	// if(!rayCastHitResult.IsSuccessful)
    	// {
    	// 	return {};
    	// }
    	//
	    // return rayCastHitResult.Entity;
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

	void AIAgentComponent::SetFactAsInteger(const String& key, int value)
	{
    	m_integerFacts[key] = value;
	}

	bool AIAgentComponent::HasIntegerFact(const String& key)
	{
    	const auto it = m_integerFacts.find(key);

    	if(it == m_integerFacts.end())
    	{
    		return false;
    	}

    	return true;
	}

	int AIAgentComponent::GetFactAsInteger(const String& key)
	{
    	const auto it = m_integerFacts.find(key);

    	if(it == m_integerFacts.end())
    	{
    		return {};
    	}

    	return m_integerFacts[key];
	}

	void AIAgentComponent::SetFactAsFloat(const String& key, float value)
	{
    	m_floatFacts[key] = value;
	}

	bool AIAgentComponent::HasFloatFact(const String& key)
	{
    	const auto it = m_floatFacts.find(key);

    	if(it == m_floatFacts.end())
    	{
    		return false;
    	}

    	return true;
	}

	float AIAgentComponent::GetFactAsFloat(const String& key)
	{
    	const auto it = m_floatFacts.find(key);

    	if(it == m_floatFacts.end())
    	{
    		return {};
    	}

    	return m_floatFacts[key];
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
            out << YAML::Key << "Is active" << YAML::Value << component.IsActive;
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
        	out << YAML::Key << "Range of vision" << YAML::Value << component.RangeOfVision;
        	out << YAML::Key << "Angle of vision" << YAML::Value << component.AngleOfVision;
        }
        
        void DeserializeData(const YAML::Node& node, AIAgentComponent& component) override
        {
        	component.IsActive = node["Is active"].as<bool>();
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
        	TryParse(node["Range of vision"], component.RangeOfVision);
        	TryParse(node["Angle of vision"], component.AngleOfVision);
        }
    };
}

