#pragma once

#include "DataTypes/String.h"
#include "DataTypes/UnorderedMap.h"
#include "BoneInfo.h"
#include "Serializers/CerealEastlMemory.h"
#include "Serializers/CerealEastlString.h"

namespace LevEngine
{
    struct SkeletonNodeData;

    class Skeleton
    {
    public:
        [[nodiscard]] Ref<SkeletonNodeData> GetRootNode() const;
        void SetRootNode(const Ref<SkeletonNodeData>& rootNode);
        [[nodiscard]] UnorderedMap<String, BoneInfo>& GetBoneInfoMap();
        
        void Serialize(const Path& path);
        void Deserialize(const Path& path);

    private:
        friend class cereal::access;
        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_BoneInfoMap);
            archive(m_RootNode);
        }
        
        UnorderedMap<String, BoneInfo> m_BoneInfoMap{};
        Ref<SkeletonNodeData> m_RootNode = nullptr;
    };
}


