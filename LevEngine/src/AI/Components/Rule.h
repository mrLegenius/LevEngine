#pragma once
#include "Scene/Entity.h"

namespace LevEngine
{
    enum class RuleConditionType
    {
        Bool,
        Number,
        Vector3,
        String,
        Entity
    };

    struct RuleCondition
    {
        String id;
        String attribute;
        String operation;
        RuleConditionType type;
    
        bool boolValue;
        float numberValue;
        Vector3 vector3Value;
        String stringValue;
        Entity entityValue;
    };
    
    class Rule
    {
    public:
        Rule(const String& ruleName, int priority);
        Rule(const std::string& ruleName, int priority);
        
        void AddCondition(const String& id, const String& attribute, const String& operation, bool value);
        void AddCondition(const String& id, const String& attribute, const String& operation, float value);
        void AddCondition(const String& id, const String& attribute, const String& operation, const Vector3& value);
        void AddCondition(const String& id, const String& attribute, const String& operation, const String& value);
        void AddCondition(const String& id, const String& attribute, const String& operation, Entity value);

        Vector<RuleCondition>& GetConditions();
        const String& GetName() const;
        int GetPriority() const;
    private:

        int m_priority;
        String m_name;
        Vector<RuleCondition> m_conditions;
    }; 
}
