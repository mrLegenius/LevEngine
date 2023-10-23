#include "levpch.h"
#include "WaypointMovement.h"

#include "Scene/SceneManager.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    WaypointMovementComponent::WaypointMovementComponent() = default;

    class WaypointMovementComponentSerializer final
        : public ComponentSerializer<WaypointMovementComponent, WaypointMovementComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "WaypointMovement"; } 

        void SerializeData(YAML::Emitter& out, const WaypointMovementComponent& component) override
        {
            out << YAML::Key << "WaypointsCount" << YAML::Value << static_cast<int>(component.entities.size());
            out << YAML::Key << "Waypoints" << YAML::Value << YAML::BeginSeq;
            for (auto entity : component.entities)
            {
                out << YAML::BeginMap;

                if (entity)
                {
                    out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
                }
                else
                {
                    out << YAML::Key << "Entity" << YAML::Value << YAML::Null;
                }
                
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;
        }
        
        void DeserializeData(YAML::Node& node, WaypointMovementComponent& component) override
        {
            const int componentCount = node["WaypointsCount"].as<int>();
            component.entities = Vector<Entity>(componentCount);

            auto waypointsNodes = node["Waypoints"];
            if (!waypointsNodes)
            {
                return;
            }

            int componentIdx = 0;
            for (auto waypoint : waypointsNodes)
            {
                auto entityNode = waypoint["Entity"];
                
                component.entities[componentIdx] = !entityNode.IsNull()
                    ? SceneManager::GetActiveScene()->GetEntityByUUID(UUID(entityNode.as<uint64_t>()))
                    : Entity();
                
                componentIdx++;
            }
        }
    };
}


