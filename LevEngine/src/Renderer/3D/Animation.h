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
        Matrix boneBindPoseTransform;
        Matrix boneCurrentTransform;
        Matrix boneModelToLocalTransform;
        String name;
        Vector<NodeData*> children;
        NodeData* parent;
    };

    class Animation
    {
        friend class AnimationLoader;
    public:
        Animation() = default;

        Bone* FindBone(const String& name);

        [[nodiscard]] double GetTicksPerSecond() const;
        [[nodiscard]] double GetDuration() const;
        [[nodiscard]] NodeData* GetRootNode();
        [[nodiscard]] const UnorderedMap<String, BoneInfo>& GetBoneIDMap() const;
        [[nodiscard]] const String& GetName() const;

    private:
        double m_Duration{};
        double m_TicksPerSecond{};
        Vector<Bone> m_Bones{};
        NodeData* m_RootNode = new NodeData();
        UnorderedMap<String, BoneInfo> m_BoneInfoMap{};
        String m_Name;
    };
}
