#include "levpch.h"
#include "Rule.h"

namespace LevEngine
{
    Rule::Rule(const String& ruleName)
    {
        name = ruleName;
    }

    Rule::Rule(const std::string& ruleName)
    {
        name = ruleName.c_str();
    }

    void Rule::AddCondition(const String& id, const String& attribute, const String& operation, bool value)
    {
        RuleCondition condition;
        condition.id = id;
        condition.attribute = attribute;
        condition.operation = operation;
        condition.type = RuleConditionType::Bool;
        condition.boolValue = value;
        conditions.push_back(condition);
    }

    void Rule::AddCondition(const String& id, const String& attribute, const String& operation, float value)
    {
        RuleCondition condition;
        condition.id = id;
        condition.attribute = attribute;
        condition.operation = operation;
        condition.type = RuleConditionType::Number;
        condition.numberValue = value;
        conditions.push_back(condition);
    }

    void Rule::AddCondition(const String& id, const String& attribute, const String& operation, const Vector3& value)
    {
        RuleCondition condition;
        condition.id = id;
        condition.attribute = attribute;
        condition.operation = operation;
        condition.type = RuleConditionType::Vector3;
        condition.vector3Value = value;
        conditions.push_back(condition);
    }

    void Rule::AddCondition(const String& id, const String& attribute, const String& operation, const String& value)
    {
        RuleCondition condition;
        condition.id = id;
        condition.attribute = attribute;
        condition.operation = operation;
        condition.type = RuleConditionType::String;
        condition.stringValue = value;
        conditions.push_back(condition);
    }

    void Rule::AddCondition(const String& id, const String& attribute, const String& operation, Entity value)
    {
        RuleCondition condition;
        condition.id = id;
        condition.attribute = attribute;
        condition.operation = operation;
        condition.type = RuleConditionType::Entity;
        condition.entityValue = value;
        conditions.push_back(condition);
    }

    Vector<RuleCondition>& Rule::GetConditions()
    {
        return conditions;
    }

    const String& Rule::GetName() const
    {
        return name;
    }
}
