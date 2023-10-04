#include "pch.h"

#include "AssetDrawer.h"
#include "Assets/DefaultAsset.h"

namespace LevEngine::Editor
{
    class DefaultAssetDrawer final : public AssetDrawer<DefaultAsset, DefaultAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "DefaultAsset"; }

        void DrawContent(Ref<DefaultAsset> assetRef) override { }
    };
}
