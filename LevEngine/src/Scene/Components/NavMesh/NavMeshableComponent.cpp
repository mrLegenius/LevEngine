#include "levpch.h"
#include "NavMeshableComponent.h"

#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    class NavMeshableComponentSerializer final : public ComponentSerializer<NavMeshableComponent, NavMeshableComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "NavMeshable"; }
        
        void SerializeData(YAML::Emitter& out, const NavMeshableComponent& component) override
        {
            out << YAML::Key << "Use in NavMesh" << YAML::Value << component.useInNavMesh;
        }
        
        void DeserializeData(YAML::Node& node, NavMeshableComponent& component) override
        {
            component.useInNavMesh = node["Use in NavMesh"].as<bool>();
        }
    };

    void NavMeshableComponent::func()
    {
        field++;
    }
}
