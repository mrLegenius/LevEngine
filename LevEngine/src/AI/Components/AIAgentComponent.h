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
        
        bool IsActive = false; 
        
        dtCrowdAgentParams* GetAgentParams() const;
        void SetAgentParams(dtCrowdAgentParams* params);

        void SetMoveTarget(Vector3 targetPos);

        Entity FindEntityInVisibleScope(int layer) const;

        void SetFactAsBool(const String&  key, bool value);
        bool HasBoolFact(const String& key);
        bool GetFactAsBool(const String& key);

        void SetFactAsInteger(const String& key, int value);
        bool HasIntegerFact(const String& key);
        int GetFactAsInteger(const String& key);
        
        void SetFactAsFloat(const String& key, float value);
        bool HasFloatFact(const String& key);
        float GetFactAsFloat(const String& key);
        
        void SetFactAsVector3(const String& key, Vector3 value);
        bool HasVector3Fact(const String& key);
        Vector3 GetFactAsVector3(const String& key);

        void SetFactAsString(const String& key, const String& value);
        bool HasStringFact(const String& key);
        String GetFactAsString(const String& key);

        float RangeOfVision = 15.0f;
        float AngleOfVision = 40.0f;

        Entity visionCollider;

    private:

        bool m_initialized = false;
        
        int m_agentIndex;
        
        Entity m_crowd;
        Entity m_selfEntity;
        
        dtCrowdAgent* m_agent;
        dtCrowdAgentParams* m_agentParams;

        Map<String, bool> m_boolFacts;
        Map<String, int> m_integerFacts;
        Map<String, float> m_floatFacts;
        Map<String, Vector3> m_vector3Facts;
        Map<String, String> m_stringFacts;
    };
}
