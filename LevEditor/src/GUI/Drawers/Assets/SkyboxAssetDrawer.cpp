#include "pch.h"

#include "AssetDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class SkyboxAssetDrawer final : public AssetDrawer<SkyboxAsset, SkyboxAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "SkyboxAsset"; }

        void DrawContent(Ref<SkyboxAsset> assetRef) override
        {
            EditorGUI::DrawTextureAsset("Left", assetRef->left);
            EditorGUI::DrawTextureAsset("Right", assetRef->right);
            EditorGUI::DrawTextureAsset("Bottom", assetRef->bottom);
            EditorGUI::DrawTextureAsset("Top", assetRef->top);
            EditorGUI::DrawTextureAsset("Back", assetRef->back);
            EditorGUI::DrawTextureAsset("Front", assetRef->front);
        }
    };
}
