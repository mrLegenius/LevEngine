#pragma once
#include "DataTypes/String.h"
#include "DataTypes/Vector.h"
#include "Bone.h"
#include "Serializers/CerealEastlString.h"

namespace LevEngine
{
    class Animation
    {
        friend class AnimationLoader;
    public:
        Animation() = default;

        Bone* FindBone(const String& name);

        [[nodiscard]] double GetTicksPerSecond() const;
        [[nodiscard]] double GetDuration() const;
        [[nodiscard]] const String& GetName() const;

        void Serialize(const Path& path);
        void Deserialize(const Path& path);

    private:
        friend class cereal::access;
        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_Duration);
            archive(m_TicksPerSecond);
            archive(m_Bones);
            archive(m_Name);
        }
        
        double m_Duration{};
        double m_TicksPerSecond{};
        Vector<Bone> m_Bones{};
        String m_Name;
    };
}
