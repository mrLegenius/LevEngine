#pragma once

#include "DataTypes/Vector.h"
#include "DataTypes/String.h"
#include "Math/Matrix.h"
#include "cereal/access.hpp"

namespace LevEngine
{
    struct SkeletonNodeData
    {
        Matrix boneBindPoseTransform;
        String name;
        Vector<Ref<SkeletonNodeData>> children;
        Ref<SkeletonNodeData> parent;

    private:
        friend class cereal::access;
        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(boneBindPoseTransform);
            archive(name);
            archive(children);
            archive(parent);
        }
    };
}
