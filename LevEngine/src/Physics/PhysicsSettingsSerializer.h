#pragma once

namespace LevEngine
{
    class PhysicsSettingsSerializer
    {
    public:
        void Serialize(YAML::Emitter& out);
        void Deserialize(const YAML::Node& node);
    };
}