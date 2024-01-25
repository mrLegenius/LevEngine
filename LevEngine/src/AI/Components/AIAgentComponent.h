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
        
        void SetFactAsBool(const Pair<String, String>&, bool value);
        bool HasBoolFact(const Pair<String, String>& key);
        bool GetFactAsBool(const Pair<String, String>& key);
        
        void SetFactAsNumber(const Pair<String, String>& key, float value);
        bool HasNumberFact(const Pair<String, String>& key);
        float GetFactAsNumber(const Pair<String, String>& key);
        
        void SetFactAsVector3(const Pair<String, String>& key, Vector3 value);
        bool HasVector3Fact(const Pair<String, String>& key);
        Vector3 GetFactAsVector3(const Pair<String, String>& key);

        void SetFactAsString(const Pair<String, String>& key, const String& value);
        bool HasStringFact(const Pair<String, String>& key);
        String GetFactAsString(const Pair<String, String>& key);

    private:

        bool m_RBSInited = false;
        
        Vector<Rule> m_rules;

        bool m_initialized = false;
        
        int m_agentIndex;
        
        Entity m_crowd;
        Entity m_selfEntity;
        
        dtCrowdAgent* m_agent;
        dtCrowdAgentParams* m_agentParams;

        Map<Pair<String, String>, bool> m_boolFacts;
        Map<Pair<String, String>, float> m_numberFacts;
        Map<Pair<String, String>, Vector3> m_vector3Facts;
        Map<Pair<String, String>, String> m_stringFacts;

        String m_lastMatchedRule;

        bool m_isRBSDirty = true;
    };
}
