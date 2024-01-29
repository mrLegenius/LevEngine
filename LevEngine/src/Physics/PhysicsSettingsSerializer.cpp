#include "levpch.h"
#include "PhysicsSettingsSerializer.h"

#include "PhysicsSettings.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
    void PhysicsSettingsSerializer::Serialize(YAML::Emitter& out)
    {
        out << YAML::Key << "PhysicsSettings" << YAML::Value << YAML::BeginMap;
        {
            out << YAML::Key << "LayerCollisionMatrix" << YAML::Flow << YAML::BeginSeq;
            for (auto i = 0; i < PhysicsSettings::m_LayerCollisionArray.size(); i++)
            {
                out << YAML::Value << static_cast<uint32_t>(PhysicsSettings::m_LayerCollisionArray[i]);
            }
            out << YAML::EndSeq;
        }
        out << YAML::EndMap; 
    }

    void PhysicsSettingsSerializer::Deserialize(const YAML::Node& node)
    {
        if (const auto physicsSettingsNode = node["PhysicsSettings"])
        {
            if (const auto layerCollisionMatrixNode = physicsSettingsNode["LayerCollisionMatrix"])
            {
                for (auto i = 0; i < PhysicsSettings::m_LayerCollisionArray.size(); i++)
                {
                    PhysicsSettings::m_LayerCollisionArray[i] =
                        static_cast<FilterLayer>(layerCollisionMatrixNode[i].as<uint32_t>());
                }
            }
        }
    }
}