#pragma once

#include "Kernel/Core.h"
#include "Asset.h"

namespace LevEngine
{
    class LEV_API AudioBankAsset final : public Asset
    {
    public:
        explicit AudioBankAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }

    protected:
        bool WriteDataToFile() const override { return false; }
        bool ReadDataFromFile() const override { return false; }

        void SerializeData(YAML::Emitter& out) override {};
        void DeserializeData(const YAML::Node& node) override {};
    };
}
