#include "levpch.h"
#include "NavMeshBoundingBox.h"

#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
    NavMeshBoundingBox::NavMeshBoundingBox()
    {
        m_positionOffset = Vector3::Zero;
        m_boxHalfExtents = Vector3::One;
    }

    Vector3 NavMeshBoundingBox::GetMinPoint() const
    {
        return { m_positionOffset.x - m_boxHalfExtents.x,
            m_positionOffset.y - m_boxHalfExtents.y,
            m_positionOffset.z - m_boxHalfExtents.z};
    }

    Vector3 NavMeshBoundingBox::GetMaxPoint() const
    {
        return {m_positionOffset.x + m_boxHalfExtents.x,
            m_positionOffset.y + m_boxHalfExtents.y,
            m_positionOffset.z + m_boxHalfExtents.z};
    }

    Vector3 NavMeshBoundingBox::GetPositionOffset() const
    {
        return m_positionOffset;
    }

    Vector3 NavMeshBoundingBox::GetBoxHalfExtents() const
    {
        return m_boxHalfExtents;
    }

    void NavMeshBoundingBox::SetPositionOffset(Vector3 positionOffset)
    {
        m_positionOffset = positionOffset;
    }

    void NavMeshBoundingBox::SetBoxHalfExtents(Vector3 boxHalfExtents)
    {
        m_boxHalfExtents = boxHalfExtents;
    }

    class NavMeshBoundingBoxSerializer final : public ComponentSerializer<NavMeshBoundingBox, NavMeshBoundingBoxSerializer>
    {
    protected:
        const char* GetKey() override { return "NavMeshBoundingBox"; }
        
        void SerializeData(YAML::Emitter& out, const NavMeshBoundingBox& component) override
        {
            out << YAML::Key << "Position offset" << YAML::Value << component.GetPositionOffset();
            out << YAML::Key << "Box half extents" << YAML::Value << component.GetBoxHalfExtents();
        }
        
        void DeserializeData(const YAML::Node& node, NavMeshBoundingBox& component) override
        {
            component.SetPositionOffset(node["Position offset"].as<Vector3>());
            component.SetBoxHalfExtents(node["Box half extents"].as<Vector3>());
        }
    };
}
