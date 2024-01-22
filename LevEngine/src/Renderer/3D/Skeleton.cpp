#include "levpch.h"
#include "Skeleton.h"
#include "SkeletonNodeData.h"
#include "cereal/archives/binary.hpp"

namespace LevEngine
{
    Ref<SkeletonNodeData> Skeleton::GetRootNode() const
    {
        return m_RootNode;
    }

    void Skeleton::SetRootNode(const Ref<SkeletonNodeData>& rootNode)
    {
        m_RootNode = rootNode;
    }

    UnorderedMap<String, BoneInfo>& Skeleton::GetBoneInfoMap()
    {
        return m_BoneInfoMap;
    }

    void Skeleton::Serialize(const Path& path)
    {
        std::ofstream os(path, std::ios::binary);
        cereal::BinaryOutputArchive archive( os );

        serialize(archive);
    }

    void Skeleton::Deserialize(const Path& path)
    {
        std::ifstream os(path, std::ios::binary);
        cereal::BinaryInputArchive archive( os );

        serialize(archive);
    }
}

