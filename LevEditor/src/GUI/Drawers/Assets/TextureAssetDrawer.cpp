#include "pch.h"

#include "AssetDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class TextureAssetDrawer final : public AssetDrawer<TextureAsset, TextureAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "TextureAsset"; }

        void DrawContent(Ref<TextureAsset> assetRef) override
        {
	        {
				const char* filterStrings[] = { "Nearest", "Linear" };
				SamplerState::MinFilter minFilter;
				SamplerState::MagFilter magFilter;
				SamplerState::MipFilter mipFilter;
				assetRef->samplerState->GetFilter(minFilter, magFilter, mipFilter);
				bool changed = false;
				changed |= GUIUtils::DrawComboBox("Min Filter", filterStrings, 2, &minFilter);
				changed |= GUIUtils::DrawComboBox("Mag Filter", filterStrings, 2, &magFilter);
				changed |= GUIUtils::DrawComboBox("Mip Filter", filterStrings, 2, &mipFilter);

				if (changed)
					assetRef->samplerState->SetFilter(minFilter, magFilter, mipFilter);
	        }

			{
				const char* wrapStrings[] = { "Repeat", "Mirror", "Clamp", "Border" };
				SamplerState::WrapMode wrapModeU;
				SamplerState::WrapMode wrapModeV;
				SamplerState::WrapMode wrapModeW;
				assetRef->samplerState->GetWrapMode(wrapModeU, wrapModeV, wrapModeW);

				bool changed = false;
				changed |= GUIUtils::DrawComboBox("Wrap Mode U", wrapStrings, 4, &wrapModeU);
				changed |= GUIUtils::DrawComboBox("Wrap Mode V", wrapStrings, 4, &wrapModeV);
				changed |= GUIUtils::DrawComboBox("Wrap Mode W", wrapStrings, 4, &wrapModeW);

				if (changed)
					assetRef->samplerState->SetWrapMode(wrapModeU, wrapModeV, wrapModeW);
			}
			
			{
				const char* compareModeStrings[] = { "None", "CompareRefToTexture" };

				SamplerState::CompareMode compareMode = assetRef->samplerState->GetCompareMode();

				bool changed = GUIUtils::DrawComboBox("Compare Mode", compareModeStrings, 2, &compareMode);

				if (changed)
					assetRef->samplerState->SetCompareMode(compareMode);
			}

			{
				const char* compareModeStrings[] = { "Never", "Less", "Equal", "LessOrEqual", "Greater", "NotEqual", "GreaterOrEqual", "Always" };

				SamplerState::CompareFunc compareMode = assetRef->samplerState->GetCompareFunction();

				bool changed = GUIUtils::DrawComboBox("Compare Function", compareModeStrings, 8, &compareMode);

				if (changed)
					assetRef->samplerState->SetCompareFunction(compareMode);
			}

			GUIUtils::DrawFloatControl("LOD Bias",
				BindGetter(&SamplerState::GetLODBias, assetRef->samplerState.get()),
				BindSetter(&SamplerState::SetLODBias, assetRef->samplerState.get()),
				1.0f, 0.0f, std::numeric_limits<float>::max());

			GUIUtils::DrawFloatControl("Min LOD",
				BindGetter(&SamplerState::GetMinLOD, assetRef->samplerState.get()),
				BindSetter(&SamplerState::SetMinLOD, assetRef->samplerState.get()),
				1.0f, 0.0f, std::numeric_limits<float>::max());

			GUIUtils::DrawFloatControl("Max LOD",
				BindGetter(&SamplerState::GetMaxLOD, assetRef->samplerState.get()),
				BindSetter(&SamplerState::SetMaxLOD, assetRef->samplerState.get()), 
				1.0f, 0.0f, std::numeric_limits<float>::max());

			GUIUtils::DrawColor3Control("Border Color", 
				BindGetter(&SamplerState::GetBorderColor, assetRef->samplerState.get()),
				BindSetter(&SamplerState::SetBorderColor, assetRef->samplerState.get()));

			GUIUtils::DrawCheckBox("Enable Anisotropic Filtering", 
				BindGetter(&SamplerState::IsAnisotropicFilteringEnabled, assetRef->samplerState.get()),
				BindSetter(&SamplerState::EnableAnisotropicFiltering, assetRef->samplerState.get()));

			GUIUtils::DrawIntControl("Max Anisotropy",
				BindGetter(&SamplerState::GetMaxAnisotropy, assetRef->samplerState.get()),
				BindSetter(&SamplerState::SetMaxAnisotropy, assetRef->samplerState.get()), 
				1, 1, 16);
        }
    };
}
