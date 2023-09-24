#include "pch.h"

#include "AssetDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class SkyboxAssetDrawer final : public AssetDrawer<SkyboxAsset, SkyboxAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "SkyboxAsset"; }

        void DrawContent(Ref<SkyboxAsset> assetRef) override
        {
            GUIUtils::DrawTextureAsset("Left", &assetRef->left);
            GUIUtils::DrawTextureAsset("Right", &assetRef->right);
            GUIUtils::DrawTextureAsset("Bottom", &assetRef->bottom);
            GUIUtils::DrawTextureAsset("Top", &assetRef->top);
            GUIUtils::DrawTextureAsset("Back", &assetRef->back);
            GUIUtils::DrawTextureAsset("Front", &assetRef->front);
        }
    };
}
