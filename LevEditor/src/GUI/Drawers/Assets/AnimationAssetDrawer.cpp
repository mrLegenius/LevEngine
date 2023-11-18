#include "pch.h"
#include "AssetDrawer.h"
#include "Assets/AnimationAsset.h"

namespace LevEngine::Editor
{
    class AnimationAssetDrawer final : public AssetDrawer<AnimationAsset, AnimationAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "AnimationAsset"; }

        void DrawContent(const Ref<AnimationAsset> assetRef) override
        {
            ImGui::LabelText("Duration: ", std::to_string(assetRef->GetDuration()).c_str());
        }
    };
}
