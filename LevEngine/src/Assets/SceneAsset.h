#pragma once

#include "Kernel/Core.h"
#include "Asset.h"

namespace LevEngine
{
    class LEV_API SceneAsset final : public Asset
    {
    public:
        SceneAsset(const Path& path, const UUID& uuid)
            : Asset(path, uuid) { }

        void Load() const;
    protected:
        [[nodiscard]] bool WriteDataToFile() const override { return false; }
        [[nodiscard]] bool ReadDataFromFile() const override { return false; }

        void SerializeData(YAML::Emitter& out) override { }
        void DeserializeData(const YAML::Node& node) override { }
    };
}



