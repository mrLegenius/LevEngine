#pragma once
#include "Asset.h"

namespace LevEngine
{
    class ScriptAsset final : public Asset
    {
    public:
        enum class Type
        {
            Undefined,
            System,
            Component
        };
        ScriptAsset(const Path& path, const UUID uuid);
        ScriptAsset(const Path& path, const UUID uuid, Type type);

        [[nodiscard]] Type GetType() const;
        void SetType(Type type);

    protected:
        [[nodiscard]] bool WriteDataToFile() const override;
        [[nodiscard]] bool ReadDataFromFile() const override;

        void SerializeData(YAML::Emitter& out) override;
        void DeserializeData(const YAML::Node& node) override;

        void SerializeMeta(YAML::Emitter& out) override;
        void DeserializeMeta(const YAML::Node& node) override;

        Type m_Type;
    };
}
