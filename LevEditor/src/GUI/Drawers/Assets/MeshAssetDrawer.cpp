#include "pch.h"

#include "AssetDrawer.h"

namespace LevEngine::Editor
{
    class MeshAssetDrawer final : public AssetDrawer<MeshAsset, MeshAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "MeshAsset"; }

        void DrawContent(Ref<MeshAsset> assetRef) override { }
    };
}
