#pragma once
#include "DataTypes/String.h"
#include "DataTypes/Vector.h"
#include "DataTypes/UnorderedMap.h"
#include "Math/Matrix.h"
#include "BoneInfo.h"
#include "Bone.h"

namespace LevEngine
{
    struct NodeData
    {
        Matrix transformation;
        String name;
        unsigned int childrenCount;
        Vector<NodeData> children;
    };

    class Animation
    {
        friend class AnimationLoader;
    public:
        Animation() = default;

        Bone* FindBone(const String& name);

        [[nodiscard]] double GetTicksPerSecond() const;
        [[nodiscard]] double GetDuration() const;
        [[nodiscard]] const NodeData& GetRootNode() const;
        [[nodiscard]] const UnorderedMap<String, BoneInfo>& GetBoneIDMap() const;

    private:
        double m_Duration{};
        double m_TicksPerSecond{};
        Vector<Bone> m_Bones{};
        NodeData m_RootNode{};
        UnorderedMap<String, BoneInfo> m_BoneInfoMap{};
    };
}
