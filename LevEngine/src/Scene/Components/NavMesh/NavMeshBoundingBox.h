#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(NavMeshBoundingBox);
	
    struct NavMeshBoundingBox
    {
        NavMeshBoundingBox();

        Vector3 GetMinPoint() const;
        Vector3 GetMaxPoint() const;
        
        Vector3 GetPositionOffset() const;
        Vector3 GetBoxHalfExtents() const;

        void SetPositionOffset(Vector3 positionOffset);
        void SetBoxHalfExtents(Vector3 boxHalfExtents);
        
    private:
        
        Vector3 m_positionOffset = Vector3::Zero;
        Vector3 m_boxHalfExtents = Vector3::One;
    };
}
