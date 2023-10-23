#pragma once
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
    class RenderSettingsSerializer
    {
    public:
        void Serialize(YAML::Emitter& out);
        void Deserialize(const YAML::Node& node);
    };
}

