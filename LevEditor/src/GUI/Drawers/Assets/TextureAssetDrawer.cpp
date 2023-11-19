﻿#include "pch.h"

#include "AssetDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class TextureAssetDrawer final : public AssetDrawer<TextureAsset, TextureAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "TextureAsset"; }

        void DrawContent(const Ref<TextureAsset> assetRef) override
        {
        	if (EditorGUI::DrawCheckBox("Is Linear", assetRef->IsLinear))
        	{
        		assetRef->CreateTexture();
        	}
        	
	        {
		        const Array<String, 2> stringValues{"Nearest", "Linear" };
				SamplerState::MinFilter minFilter;
				SamplerState::MagFilter magFilter;
				SamplerState::MipFilter mipFilter;
				assetRef->SamplerState->GetFilter(minFilter, magFilter, mipFilter);
				bool changed = false;
				changed |= EditorGUI::DrawComboBox("Min Filter", stringValues, minFilter);
				changed |= EditorGUI::DrawComboBox("Mag Filter", stringValues, magFilter);
				changed |= EditorGUI::DrawComboBox("Mip Filter", stringValues, mipFilter);

				if (changed)
					assetRef->SamplerState->SetFilter(minFilter, magFilter, mipFilter);
	        }

			{
        		const Array<String, 4> stringValues{"Repeat", "Mirror", "Clamp", "Border" };
				SamplerState::WrapMode wrapModeU;
				SamplerState::WrapMode wrapModeV;
				SamplerState::WrapMode wrapModeW;
				assetRef->SamplerState->GetWrapMode(wrapModeU, wrapModeV, wrapModeW);

				bool changed = false;
				changed |= EditorGUI::DrawComboBox("Wrap Mode U", stringValues, wrapModeU);
				changed |= EditorGUI::DrawComboBox("Wrap Mode V", stringValues, wrapModeV);
				changed |= EditorGUI::DrawComboBox("Wrap Mode W", stringValues, wrapModeW);

				if (changed)
					assetRef->SamplerState->SetWrapMode(wrapModeU, wrapModeV, wrapModeW);
			}
			
			{
        		const Array<String, 2> stringValues{"None", "CompareRefToTexture" };

				SamplerState::CompareMode compareMode = assetRef->SamplerState->GetCompareMode();

		        if (EditorGUI::DrawComboBox("Compare Mode", stringValues, compareMode))
					assetRef->SamplerState->SetCompareMode(compareMode);
			}

			{
        		const Array<String, 8> stringValues{"Never", "Less",
        			"Equal", "LessOrEqual",
        			"Greater", "NotEqual",
        			"GreaterOrEqual", "Always" };

				SamplerState::CompareFunc compareMode = assetRef->SamplerState->GetCompareFunction();

				if (EditorGUI::DrawComboBox("Compare Function", stringValues, compareMode))
					assetRef->SamplerState->SetCompareFunction(compareMode);
			}

			EditorGUI::DrawFloatControl("LOD Bias",
				BindGetter(&SamplerState::GetLODBias, assetRef->SamplerState.get()),
				BindSetter(&SamplerState::SetLODBias, assetRef->SamplerState.get()),
				1.0f, 0.0f, std::numeric_limits<float>::max());

			EditorGUI::DrawFloatControl("Min LOD",
				BindGetter(&SamplerState::GetMinLOD, assetRef->SamplerState.get()),
				BindSetter(&SamplerState::SetMinLOD, assetRef->SamplerState.get()),
				1.0f, 0.0f, std::numeric_limits<float>::max());

			EditorGUI::DrawFloatControl("Max LOD",
				BindGetter(&SamplerState::GetMaxLOD, assetRef->SamplerState.get()),
				BindSetter(&SamplerState::SetMaxLOD, assetRef->SamplerState.get()), 
				1.0f, 0.0f, std::numeric_limits<float>::max());

			EditorGUI::DrawColor3Control("Border Color", 
				BindGetter(&SamplerState::GetBorderColor, assetRef->SamplerState.get()),
				BindSetter(&SamplerState::SetBorderColor, assetRef->SamplerState.get()));

			EditorGUI::DrawCheckBox("Enable Anisotropic Filtering", 
			                        BindGetter(&SamplerState::IsAnisotropicFilteringEnabled, assetRef->SamplerState.get()),
			                        BindSetter(&SamplerState::EnableAnisotropicFiltering, assetRef->SamplerState.get()));

			EditorGUI::DrawIntControl("Max Anisotropy",
				BindGetter(&SamplerState::GetMaxAnisotropy, assetRef->SamplerState.get()),
				BindSetter(&SamplerState::SetMaxAnisotropy, assetRef->SamplerState.get()), 
				1, 1, 16);
        }
    };
}
