#pragma once
#include "DataTypes/String.h"
#include "DataTypes/Vector.h"
#include "DataTypes/UnorderedMap.h"
#include "Math/Matrix.h"
#include "BoneInfo.h"
#include "Bone.h"

namespace LevEngine
{
    class Skeleton;

    struct SkeletonNodeData
    {
        Matrix boneBindPoseTransform;
        String name;
        Vector<SkeletonNodeData*> children;
        SkeletonNodeData* parent;
    };

    class Animation
    {
        friend class AnimationLoader;
    public:
        Animation() = default;

        Bone* FindBone(const String& name);

        [[nodiscard]] double GetTicksPerSecond() const;
        [[nodiscard]] double GetDuration() const;
        [[nodiscard]] SkeletonNodeData* GetRootNode();
        [[nodiscard]] const UnorderedMap<String, BoneInfo>& GetBoneIDMap() const;
        [[nodiscard]] const String& GetName() const;

    private:
        double m_Duration{};
        double m_TicksPerSecond{};
        Vector<Bone> m_Bones{};
        String m_Name;
        Ref<Skeleton> m_Skeleton;
    };
}
