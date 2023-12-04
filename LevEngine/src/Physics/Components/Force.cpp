#include "levpch.h"
#include "Force.h"

#include "Rigidbody.h"
#include "Physics/PhysicsUtils.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    Force::Type Force::GetForceType() const
    {
        return m_Type;
    }

    void Force::SetForceType(const Type& type)
    {
        m_Type = type;
    }


    bool Force::IsCompletedForce() const
    {
        return m_IsCompletedForce;
    }

    void Force::CompleteForce(const bool flag)
    {
        m_IsCompletedForce = flag;
    }
    

    
    Vector3 Force::GetLinearForce() const
    {
        return m_LinearForce;
    }

    void Force::SetLinearForce(const Vector3 value)
    {
        m_LinearForce = value;
    }

    Vector3 Force::GetAngularForce() const
    {
        return m_AngularForce;
    }

    void Force::SetAngularForce(const Vector3 value)
    {
        m_AngularForce = value;
    }

    
    
    void Force::ApplyForce(const Rigidbody& rigidbody)
    {
        if (rigidbody.GetActor() != NULL)
        {
            if (const auto rigidDynamic = reinterpret_cast<physx::PxRigidDynamic*>(rigidbody.GetActor()))
            {
                if (m_Type == Type::Force)
                {
                    rigidDynamic->addForce(PhysicsUtils::FromVector3ToPxVec3(m_LinearForce), physx::PxForceMode::eFORCE);
                    rigidDynamic->addTorque(PhysicsUtils::FromVector3ToPxVec3(m_AngularForce), physx::PxForceMode::eFORCE);
                }
                else if (m_Type == Type::Impulse && !m_IsCompletedForce)
                {
                    rigidDynamic->addForce(PhysicsUtils::FromVector3ToPxVec3(m_LinearForce), physx::PxForceMode::eIMPULSE);
                    rigidDynamic->addTorque(PhysicsUtils::FromVector3ToPxVec3(m_AngularForce), physx::PxForceMode::eIMPULSE);
                    m_IsCompletedForce = true;
                }
                else if (m_Type == Type::Velocity && !m_IsCompletedForce)
                {
                    rigidDynamic->addForce(PhysicsUtils::FromVector3ToPxVec3(m_LinearForce), physx::PxForceMode::eVELOCITY_CHANGE);
                    rigidDynamic->addTorque(PhysicsUtils::FromVector3ToPxVec3(m_AngularForce), physx::PxForceMode::eVELOCITY_CHANGE);
                    m_IsCompletedForce = true;
                }
                else if (m_Type == Type::Acceleration)
                {
                    rigidDynamic->addForce(PhysicsUtils::FromVector3ToPxVec3(m_LinearForce), physx::PxForceMode::eACCELERATION);
                    rigidDynamic->addTorque(PhysicsUtils::FromVector3ToPxVec3(m_AngularForce), physx::PxForceMode::eACCELERATION);
                }
            } 
        }
    }


    
    class ForceSerializer final : public ComponentSerializer<Force, ForceSerializer>
    {
    protected:
        const char* GetKey() override { return "Force"; }

        void SerializeData(YAML::Emitter& out, const Force& component) override
        {
            out << YAML::Key << "Force Type" << YAML::Value << static_cast<int>(component.GetForceType());
            
            out << YAML::Key << "Linear Force" << YAML::Value << component.GetLinearForce();
            out << YAML::Key << "Angular Force" << YAML::Value << component.GetAngularForce();
        }

        void DeserializeData(YAML::Node& node, Force& component) override
        {
            
            if (const auto forceTypeNode = node["Force Type"])
            {
                component.SetForceType(static_cast<Force::Type>(forceTypeNode.as<int>()));
            }

            if (const auto linearForceNode = node["Linear Force"])
            {
                component.SetLinearForce(linearForceNode.as<Vector3>());
            }

            if (const auto angularForceNode = node["Angular Force"])
            {
                component.SetAngularForce(angularForceNode.as<Vector3>());
            }
        } 
    };
}
