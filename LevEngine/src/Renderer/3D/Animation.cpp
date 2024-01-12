#include "levpch.h"
#include "Animation.h"

namespace LevEngine
{
    Bone* Animation::FindBone(const String& name)
    {
        auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
            [&](const Bone& Bone)
        {
            return Bone.GetBoneName() == name;
        });

        if (iter == m_Bones.end())
        {
            return nullptr;
        }
        else
        {
            return &(*iter);
        }
    }

    double Animation::GetTicksPerSecond() const
    {
        return m_TicksPerSecond;
    }

    double Animation::GetDuration() const
    {
        return m_Duration;
    }

    SkeletonNodeData* Animation::GetRootNode()
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

