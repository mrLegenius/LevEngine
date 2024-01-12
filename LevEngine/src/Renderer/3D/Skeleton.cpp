#include "levpch.h"
#include "Skeleton.h"

namespace LevEngine
{
    SkeletonNodeData* Skeleton::GetRootNode() const
    {
        return m_RootNode;
    }

    void Skeleton::SetRootNode(SkeletonNodeData* rootNode)
    {
        m_RootNode = rootNode;
    }

    UnorderedMap<String, BoneInfo>& Skeleton::GetBoneInfoMap()
    {
        return m_BoneInfoMap;
    }
}

