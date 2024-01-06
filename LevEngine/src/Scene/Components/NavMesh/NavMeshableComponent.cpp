#include "levpch.h"
#include "NavMeshableComponent.h"

#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    NavMeshableComponent::NavMeshableComponent() = default;
    
    class NavMeshableComponentSerializer final : public ComponentSerializer<NavMeshableComponent, NavMeshableComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "NavMeshable"; }
        
        void SerializeData(YAML::Emitter& out, const NavMeshableComponent& component) override
        {
            out << YAML::Key << "Use in nav mesh" << YAML::Value << component.UseInNavMesh;
            out << YAML::Key << "Field" << YAML::Value << component.m_Field;
        }
        
        void DeserializeData(YAML::Node& node, NavMeshableComponent& component) override
        {
            component.UseInNavMesh = node["Use in nav mesh"].as<bool>();
            component.m_Field = node["Field"].as<int>();
        }
    };
}
