#pragma once

namespace LevEngine
{
    class RenderSettingsSerializer
    {
    public:
        void Serialize(YAML::Emitter& out);
        void Deserialize(const YAML::Node& node);
    };
}