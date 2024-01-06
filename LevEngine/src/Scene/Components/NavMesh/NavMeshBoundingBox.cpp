#include "levpch.h"
#include "NavMeshBoundingBox.h"

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

    void NavMeshBoundingBox::SetPositionOffset(Vector3 positionOffset)
    {
        m_positionOffset = positionOffset;
    }

    void NavMeshBoundingBox::SetBoxHalfExtents(Vector3 boxHalfExtents)
    {
        m_boxHalfExtents = boxHalfExtents;
    }
}
