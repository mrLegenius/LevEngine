#pragma once
#include "DataTypes/String.h"
#include "DataTypes/Vector.h"
#include "Bone.h"

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

    private:
        double m_Duration{};
        double m_TicksPerSecond{};
        Vector<Bone> m_Bones{};
        String m_Name;
    };
}
