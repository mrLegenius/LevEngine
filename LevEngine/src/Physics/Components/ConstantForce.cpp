#include "levpch.h"
#include "ConstantForce.h"

#include "Rigidbody.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    Vector3 ConstantForce::GetForce() const
    {
        return m_Force;
    }

    void ConstantForce::SetForce(const Vector3 value)
    {
        m_Force = value;
    }

    Vector3 ConstantForce::GetTorque() const
    {
        return m_Torque;
    }

    void ConstantForce::SetTorque(const Vector3 value)
    {
        m_Torque = value;
    }


    
    class ForceSerializer final : public ComponentSerializer<ConstantForce, ForceSerializer>
    {
    protected:
        const char* GetKey() override { return "Force"; }

        void SerializeData(YAML::Emitter& out, const ConstantForce& component) override
        {
            out << YAML::Key << "Force" << YAML::Value << component.GetForce();
            out << YAML::Key << "Torque" << YAML::Value << component.GetTorque();
        }

        void DeserializeData(const YAML::Node& node, ConstantForce& component) override
        {
            if (const auto linearForceNode = node["Force"])
            {
                component.SetForce(linearForceNode.as<Vector3>());
            }

            if (const auto angularForceNode = node["Torque"])
            {
                component.SetTorque(angularForceNode.as<Vector3>());
            }
        } 
    };
}