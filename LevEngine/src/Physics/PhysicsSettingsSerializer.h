#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class LEV_API PhysicsSettingsSerializer
    {
    public:
        void Serialize(YAML::Emitter& out);
        void Deserialize(const YAML::Node& node);
    };
}