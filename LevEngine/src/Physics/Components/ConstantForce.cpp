#include "levpch.h"
#include "ConstantForce.h"

#include "Rigidbody.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    void ConstantForce::UpdateConstantForces(entt::registry& registry)
    {
        const auto constantForceView = registry.view<Rigidbody, ConstantForce>();
        for (const auto entity : constantForceView)
        {
            auto [constantForceRigidbody, constantForce] = constantForceView.get<Rigidbody, ConstantForce>(entity);

            if (constantForceRigidbody.GetActor() != NULL)
            {
                constantForceRigidbody.AddForce(constantForce.GetForce(), Rigidbody::ForceMode::Force);
                constantForceRigidbody.AddTorque(constantForce.GetTorque(), Rigidbody::ForceMode::Force);
            }
        }
    }


    
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

        void DeserializeData(YAML::Node& node, ConstantForce& component) override
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