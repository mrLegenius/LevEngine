#include "levpch.h"
#include "SkeletonAsset.h"

namespace LevEngine
{
    const Ref<Skeleton>& SkeletonAsset::GetSkeleton() const
    {
        return m_Skeleton;
    }
}

