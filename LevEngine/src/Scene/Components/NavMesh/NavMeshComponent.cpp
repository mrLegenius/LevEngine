#include "levpch.h"
#include "NavMeshComponent.h"

#include "NavMeshableComponent.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"

namespace LevEngine
{
    void NavMeshComponent::Build()
    {
        memset(&m_Config, 0, sizeof(m_Config));
        m_Config.cs = m_CellSize;
        m_Config.ch = m_CellHeight;
        m_Config.walkableSlopeAngle = m_AgentMaxSlope;
        m_Config.walkableHeight = static_cast<int>(ceilf(m_AgentHeight / m_Config.ch));
        m_Config.walkableClimb = static_cast<int>(floorf(m_AgentMaxClimb / m_Config.ch));
        m_Config.walkableRadius = static_cast<int>(ceilf(m_AgentRadius / m_Config.cs));
        m_Config.maxEdgeLen = static_cast<int>(m_EdgeMaxLen / m_CellSize);
        m_Config.maxSimplificationError = m_EdgeMaxError;
        m_Config.minRegionArea = static_cast<int>(rcSqr(m_RegionMinSize));		// Note: area = size*size
        m_Config.mergeRegionArea = static_cast<int>(rcSqr(m_RegionMergeSize));	// Note: area = size*size
        m_Config.maxVertsPerPoly = static_cast<int>(m_VertsPerPoly);
        m_Config.detailSampleDist = m_DetailSampleDist < 0.9f ? 0 : m_CellSize * m_DetailSampleDist;
        m_Config.detailSampleMaxError = m_CellHeight * m_DetailSampleMaxError;
        
        auto& registry = SceneManager::GetActiveScene()->GetRegistry();
        const auto view = registry.view<NavMeshableComponent, MeshRendererComponent>();
        
        for (const auto entity : view)
        {
            auto& navMeshableComponent = view.get<NavMeshableComponent>(entity);
            if(!navMeshableComponent.useInNavMesh)
            {
                return;
            }
            auto& meshRenderComponent = view.get<MeshRendererComponent>(entity);
            
            auto& vertices = meshRenderComponent.mesh->GetMesh()->GetVertices();

            for (Vector3 vertex : vertices)
            {
                // NavMeshInfo* info = new NavMeshInfo();
                // CrowdTool* crowdTool = new CrowdTool();
                // NavigationSystem* navSystem = new NavigationSystem();
                //
                // GeometryInfo* geometry = new GeometryInfo();
                // geometry->Verts = ;
                // geometry->VertexCount = ;
                // geometry->Triangles = ;
                // geometry->TrianglesCount = ;
                //
                // info->Init(geometry);
                // navSystem->HandleBuild(info);
                //
                // crowdTool->init(navSystem);
            }
        }
    }
    
    class NavMeshComponentSerializer final : public ComponentSerializer<NavMeshComponent, NavMeshComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "NavMesh"; }
        
        void SerializeData(YAML::Emitter& out, const NavMeshComponent& component) override
        {
            out << YAML::Key << "Cell size" << YAML::Value << component.m_CellSize;
            out << YAML::Key << "Cell Height" << YAML::Value << component.m_CellHeight;
            out << YAML::Key << "Agent Height" << YAML::Value << component.m_AgentHeight;
            out << YAML::Key << "Agent radius" << YAML::Value << component.m_AgentRadius;
            out << YAML::Key << "Agent max climb" << YAML::Value << component.m_AgentMaxClimb;
            out << YAML::Key << "Agent max slope" << YAML::Value << component.m_AgentMaxSlope;
            out << YAML::Key << "Region min size" << YAML::Value << component.m_RegionMinSize;
            out << YAML::Key << "Region merge size" << YAML::Value << component.m_RegionMergeSize;
            out << YAML::Key << "Edge max len" << YAML::Value << component.m_EdgeMaxLen;
            out << YAML::Key << "Edge max error" << YAML::Value << component.m_EdgeMaxError;
            out << YAML::Key << "Verts per poly" << YAML::Value << component.m_VertsPerPoly;
            out << YAML::Key << "Detail sample dist" << YAML::Value << component.m_DetailSampleDist;
            out << YAML::Key << "Detail sample max error" << YAML::Value << component.m_DetailSampleMaxError;
            out << YAML::Key << "Partition type" << YAML::Value << component.m_PartitionType;
        }
        
        void DeserializeData(YAML::Node& node, NavMeshComponent& component) override
        {
            component.m_CellSize = node["Cell size"].as<float>();
            component.m_CellHeight = node["Cell Height"].as<float>();
            component.m_AgentHeight = node["Agent Height"].as<float>();
            component.m_AgentRadius = node["Agent radius"].as<float>();
            component.m_AgentMaxClimb = node["Agent max climb"].as<float>();
            component.m_AgentMaxSlope = node["Agent max slope"].as<float>();
            component.m_RegionMinSize = node["Region min size"].as<float>();
            component.m_RegionMergeSize = node["Region merge size"].as<float>();
            component.m_EdgeMaxLen = node["Edge max len"].as<float>();
            component.m_EdgeMaxError = node["Edge max error"].as<float>();
            component.m_VertsPerPoly = node["Verts per poly"].as<float>();
            component.m_DetailSampleDist = node["Detail sample dist"].as<float>();
            component.m_DetailSampleMaxError = node["Detail sample max error"].as<float>();
            component.m_PartitionType = node["Partition type"].as<int>();
        }
    };
}
