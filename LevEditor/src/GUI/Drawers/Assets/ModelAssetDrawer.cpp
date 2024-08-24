#include "pch.h"

#include "AssetDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class ModelAssetDrawer final : public AssetDrawer<ModelAsset, ModelAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Model"; }

        void DrawContent(Ref<ModelAsset> asset) override
        {
            EditorGUI::DrawFloatControl("Scale", asset->Scale);
        }
    };
}
