#pragma once
#pragma once
#include <DetourCrowd.h>

#include "Rule.h"
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
        static void OnDestroy(Entity entity);
        void OnComponentDestroy();
        
        
        Entity GetCrowd() const;

        int GetIndexInCrowd() const;

        bool isActive = false; 

        void AddRule(const Rule& newRule);
        void InitRBS();
        bool IsRBSInited();

        const String& Match();
        
        dtCrowdAgentParams* GetAgentParams() const;
        void SetAgentParams(dtCrowdAgentParams* params);

        void SetMoveTarget(Vector3 targetPos);
        
        void SetFactAsBool(const String&  key, bool value);
        bool HasBoolFact(const String& key);
        bool GetFactAsBool(const String& key);
        
        void SetFactAsNumber(const String& key, float value);
        bool HasNumberFact(const String& key);
        float GetFactAsNumber(const String& key);
        
        void SetFactAsVector3(const String& key, Vector3 value);
        bool HasVector3Fact(const String& key);
        Vector3 GetFactAsVector3(const String& key);

        void SetFactAsString(const String& key, const String& value);
        bool HasStringFact(const String& key);
        String GetFactAsString(const String& key);

    private:

        bool m_RBSInited = false;
        
        Vector<Rule> m_rules;

        bool m_initialized = false;
        
        int m_agentIndex;
        
        Entity m_crowd;
        Entity m_selfEntity;
        
        dtCrowdAgent* m_agent;
        dtCrowdAgentParams* m_agentParams;

        Map<String, bool> m_boolFacts;
        Map<String, float> m_numberFacts;
        Map<String, Vector3> m_vector3Facts;
        Map<String, String> m_stringFacts;
    };
}
