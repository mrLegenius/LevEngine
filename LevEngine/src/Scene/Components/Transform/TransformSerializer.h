#pragma once

#include "Kernel/Core.h"
namespace LevEngine
{
    struct Transform;

    class LEV_API TransformSerializer
    {
    public:
        static void SerializeData(YAML::Emitter& out, const Transform& component);
        static void DeserializeData(const YAML::Node& entityNode, Transform& component);
    };
}
