#pragma once

namespace LevEngine
{
    enum class RuleConditionType
    {
        Bool,
        Number,
        Vector3,
        String
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
    };
    
    class Rule
    {
    public:
        Rule(const String& ruleName);
        Rule(const std::string& ruleName);
        
        void AddCondition(const String& id, const String& conditionName, const String& operation, bool value);
        void AddCondition(const String& id, const String& conditionName, const String& operation, float value);
        void AddCondition(const String& id, const String& conditionName, const String& operation, const Vector3& value);
        void AddCondition(const String& id, const String& conditionName, const String& operation, const String& value);

        Vector<RuleCondition>& GetConditions();
        const String& GetName() const;
    private:
        
        String name;
        Vector<RuleCondition> conditions;
    }; 
}