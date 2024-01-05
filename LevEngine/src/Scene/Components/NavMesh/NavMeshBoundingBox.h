#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(NavMeshBoundingBox);
	
    struct NavMeshBoundingBox
    {
        NavMeshBoundingBox();
        NavMeshBoundingBox(const NavMeshBoundingBox&) = default;

        Vector3 GetMinPoint() const;
        Vector3 GetMaxPoint() const;
        
        Vector3 GetPositionOffset() const {return m_positionOffset; }
        Vector3 GetBoxHalfExtents() const {return m_boxHalfExtents; }

        void SetPositionOffset(Vector3 positionOffset);
        void SetBoxHalfExtents(Vector3 boxHalfExtents);
        
    private:
        
        Vector3 m_positionOffset;
        Vector3 m_boxHalfExtents;
    };
}
