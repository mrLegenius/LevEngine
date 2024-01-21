#pragma once

#include "DataTypes/String.h"
#include "DataTypes/UnorderedMap.h"
#include "BoneInfo.h"

namespace LevEngine
{
    struct SkeletonNodeData;

    class Skeleton
    {
    public:
        [[nodiscard]] SkeletonNodeData* GetRootNode() const;
        void SetRootNode(SkeletonNodeData*);
        [[nodiscard]] UnorderedMap<String, BoneInfo>& GetBoneInfoMap();

    private:
        UnorderedMap<String, BoneInfo> m_BoneInfoMap{};
        SkeletonNodeData* m_RootNode = nullptr;
    };
}


