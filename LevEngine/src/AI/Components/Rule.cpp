#include "levpch.h"
#include "Rule.h"

namespace LevEngine
{
    Rule::Rule(const String& ruleName, int priority)
    {
        m_name = ruleName;
        m_priority = priority;
    }

    Rule::Rule(const std::string& ruleName, int priority)
    {
        m_name = ruleName.c_str();
        m_priority = priority;
    }

    void Rule::AddCondition(const String& id, const String& attribute, const String& operation, bool value)
    {
        RuleCondition condition;
        condition.id = id;
        condition.attribute = attribute;
        condition.operation = operation;
        condition.type = RuleConditionType::Bool;
        condition.boolValue = value;
        m_conditions.push_back(condition);
    }

    void Rule::AddCondition(const String& id, const String& attribute, const String& operation, float value)
    {
        RuleCondition condition;
        condition.id = id;
        condition.attribute = attribute;
        condition.operation = operation;
        condition.type = RuleConditionType::Number;
        condition.numberValue = value;
        m_conditions.push_back(condition);
    }

    void Rule::AddCondition(const String& id, const String& attribute, const String& operation, const Vector3& value)
    {
        RuleCondition condition;
        condition.id = id;
        condition.attribute = attribute;
        condition.operation = operation;
        condition.type = RuleConditionType::Vector3;
        condition.vector3Value = value;
        m_conditions.push_back(condition);
    }

    void Rule::AddCondition(const String& id, const String& attribute, const String& operation, const String& value)
    {
        RuleCondition condition;
        condition.id = id;
        condition.attribute = attribute;
        condition.operation = operation;
        condition.type = RuleConditionType::String;
        condition.stringValue = value;
        m_conditions.push_back(condition);
    }

    void Rule::AddCondition(const String& id, const String& attribute, const String& operation, Entity value)
    {
        RuleCondition condition;
        condition.id = id;
        condition.attribute = attribute;
        condition.operation = operation;
        condition.type = RuleConditionType::Entity;
        condition.entityValue = value;
        m_conditions.push_back(condition);
    }

    Vector<RuleCondition>& Rule::GetConditions()
    {
        return m_conditions;
    }

    const String& Rule::GetName() const
    {
        return m_name;
    }

    int Rule::GetPriority() const
    {
        return m_priority;
    }
}
