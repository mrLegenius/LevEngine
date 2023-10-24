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
            auto changed = false;
            changed |= EditorGUI::DrawTextureAsset("Left", assetRef->left);
            changed |= EditorGUI::DrawTextureAsset("Right", assetRef->right);
            changed |= EditorGUI::DrawTextureAsset("Bottom", assetRef->bottom);
            changed |= EditorGUI::DrawTextureAsset("Top", assetRef->top);
            changed |= EditorGUI::DrawTextureAsset("Back", assetRef->back);
            changed |= EditorGUI::DrawTextureAsset("Front", assetRef->front);

            if (changed)
                assetRef->InitTexture();
        }
    };
}
