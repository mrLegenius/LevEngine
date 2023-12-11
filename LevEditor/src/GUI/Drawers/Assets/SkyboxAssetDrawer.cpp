#include "pch.h"

#include "AssetDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class SkyboxAssetDrawer final : public AssetDrawer<SkyboxAsset, SkyboxAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "SkyboxAsset"; }

        void DrawContent(const Ref<SkyboxAsset> assetRef) override
        {
            EditorGUI::DrawTextureAsset("Texture", assetRef->HDRTexture);
        }
    };
}
