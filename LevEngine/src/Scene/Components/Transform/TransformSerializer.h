#pragma once
namespace LevEngine
{
    struct Transform;

    class TransformSerializer
    {
    public:
        static void SerializeData(YAML::Emitter& out, const Transform& component);
        static void DeserializeData(const YAML::Node& entityNode, Transform& component);
    };
}
