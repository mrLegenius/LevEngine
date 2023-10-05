#pragma once
#include "Asset.h"

namespace LevEngine
{
    class MaterialAsset : public Asset
    {
    public:
        virtual Material& GetMaterial() = 0;

        [[nodiscard]] Ref<Texture> GetIcon() const override { return Icons::Material(); }
	
    protected:
        explicit MaterialAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }
    };
}
