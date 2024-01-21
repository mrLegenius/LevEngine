#pragma once

#include "DataTypes/Vector.h"
#include "DataTypes/String.h"
#include "Math/Matrix.h"

namespace LevEngine
{
    struct SkeletonNodeData
    {
        Matrix boneBindPoseTransform;
        String name;
        Vector<SkeletonNodeData*> children;
        SkeletonNodeData* parent;
    };
}
