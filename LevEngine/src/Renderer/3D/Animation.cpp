#include "levpch.h"
#include "Animation.h"

namespace LevEngine
{
    Bone* Animation::FindBone(const String& name)
    {
        auto iter = std::ranges::find_if(m_Bones,
        [&](const Bone& Bone){ return Bone.GetBoneName() == name; });

        return iter == m_Bones.end() ? nullptr : iter;
    }

    double Animation::GetTicksPerSecond() const
    {
        return m_TicksPerSecond;
    }

    double Animation::GetDuration() const
    {
        return m_Duration;
    }

    NodeData* Animation::GetRootNode() const
    {
        return m_RootNode;
    }

    const UnorderedMap<String, BoneInfo>& Animation::GetBoneIDMap() const
    {
        return m_BoneInfoMap;
    }

    const String& Animation::GetName() const
    {
        return m_Name;
    }
}

