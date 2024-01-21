#include "levpch.h"
#include "SkeletonAsset.h"

namespace LevEngine
{
    SkeletonAsset::SkeletonAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }

    const Ref<Skeleton>& SkeletonAsset::GetSkeleton() const
    {
        return m_Skeleton;
    }

    void SkeletonAsset::Init(const Ref<Skeleton>& skeleton)
    {
        m_Skeleton = skeleton;
    }

    void SkeletonAsset::SerializeData(YAML::Emitter& out)
    {
        
    }

    void SkeletonAsset::DeserializeData(const YAML::Node& node)
    {
        
    }
}

