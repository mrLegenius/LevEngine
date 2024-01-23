#include "levpch.h"
#include "Animation.h"

#include "cereal/archives/binary.hpp"

namespace LevEngine
{
    Bone* Animation::FindBone(const String& name)
    {
        const auto iter = std::find_if(m_Bones.begin(), m_Bones.end(), [&](const Bone& Bone)
        {
           return Bone.GetBoneName() == name;
        });

        if (iter == m_Bones.end())
        {
            return nullptr;
        }

        return &(*iter);
    }

    double Animation::GetTicksPerSecond() const
    {
        return m_TicksPerSecond;
    }

    double Animation::GetDuration() const
    {
        return m_Duration;
    }

    const String& Animation::GetName() const
    {
        return m_Name;
    }
    
    void Animation::Serialize(const Path& path)
    {
        std::ofstream os(path, std::ios::binary);
        cereal::BinaryOutputArchive archive( os );

        serialize(archive);
    }

    void Animation::Deserialize(const Path& path)
    {
        std::ifstream os(path, std::ios::binary);
        cereal::BinaryInputArchive archive( os );

        serialize(archive);
    }
}

