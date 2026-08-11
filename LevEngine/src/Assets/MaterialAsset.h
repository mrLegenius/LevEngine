#pragma once

#include "Kernel/Core.h"
#include "Asset.h"

namespace LevEngine
{
    class Material;

    class LEV_API MaterialAsset : public Asset
    {
    public:
        virtual Material& GetMaterial() = 0;

        [[nodiscard]] Ref<Texture> GetIcon() const override;
    protected:
        explicit MaterialAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }
    };
}
