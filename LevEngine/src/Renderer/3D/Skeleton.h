#pragma once

namespace LevEngine
{
    struct SkeletonNodeData;
    struct BoneInfo;

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


