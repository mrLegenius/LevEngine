#include "pch.h"

#include "AssetDrawer.h"
#include "Assets/AudioBankAsset.h"

namespace LevEngine::Editor
{
    class AudioBankAssetDrawer final : public AssetDrawer<AudioBankAsset, AudioBankAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "AudioBank"; }

        void DrawContent(Ref<AudioBankAsset> assetRef) override { }
    };
}
