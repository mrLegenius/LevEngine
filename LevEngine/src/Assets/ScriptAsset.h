#pragma once
#include "Asset.h"

namespace LevEngine
{
    class ScriptAsset : public Asset
    {
    public:
        enum class Type : uint8_t
        {
            Undefined,
            System,
            Component
        };
        ScriptAsset(const Path& path, const UUID uuid);
        ScriptAsset(const Path& path, const UUID uuid, Type type);

        [[nodiscard]] Type GetType() const;

    protected:
        [[nodiscard]] bool WriteDataToFile() const override;
        [[nodiscard]] bool ReadDataFromFile() const override;

        void SerializeData(YAML::Emitter& out) override;
        void DeserializeData(YAML::Node& node) override;

        void SerializeMeta(YAML::Emitter& out) override;
        void DeserializeMeta(YAML::Node& node) override;

        Type m_Type;
    };
}
