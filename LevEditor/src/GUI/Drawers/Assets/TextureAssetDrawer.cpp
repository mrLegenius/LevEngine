#include "pch.h"

#include "AssetDrawer.h"
#include "Assets/TextureAsset.h"
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
			    assetRef->CreateTexture();
        	
		    DrawFilters(assetRef);
        	DrawWrapMode(assetRef);
		    DrawCompare(assetRef);
        	DrawMipMaps(assetRef);

		    EditorGUI::DrawColor3Control("Border Color", 
		                                 BindGetter(&SamplerState::GetBorderColor, assetRef->SamplerState.get()),
		                                 BindSetter(&SamplerState::SetBorderColor, assetRef->SamplerState.get()));

		    DrawFiltering(assetRef);
	    }

	    static void DrawFilters(const Ref<TextureAsset>& assetRef)
	    {
		    Array<String, 2> filterValues { "Nearest", "Linear", };

        	EditorGUI::DrawCheckBox("Separate Filters", assetRef->SeparateFilters);
        	
		    if (assetRef->SeparateFilters)
		    {
			    SamplerState::Filter filter;
			    assetRef->SamplerState->GetFilter(filter, filter, filter);
			    bool changed = false;
			    changed |= EditorGUI::DrawComboBox("Filters", filterValues, filter);

			    if (changed)
				    assetRef->SamplerState->SetFilter(filter, filter, filter);
		    }
		    else
		    {
			    SamplerState::Filter minFilter;
			    SamplerState::Filter magFilter;
			    SamplerState::Filter mipFilter;
			    assetRef->SamplerState->GetFilter(minFilter, magFilter, mipFilter);
			    bool changed = false;
			    changed |= EditorGUI::DrawComboBox("Min Filter", filterValues, minFilter);
			    changed |= EditorGUI::DrawComboBox("Mag Filter", filterValues, magFilter);
			    changed |= EditorGUI::DrawComboBox("Mip Filter", filterValues, mipFilter);

			    if (changed)
				    assetRef->SamplerState->SetFilter(minFilter, magFilter, mipFilter);
		    }
	    }

	    static void DrawWrapMode(const Ref<TextureAsset>& assetRef)
	    {
		    const Array<String, 4> stringValues{"Repeat", "Mirror", "Clamp", "Border" };

        	EditorGUI::DrawCheckBox("Separate Wrap Mode", assetRef->SeparateWrapMode);
        	
        	if (assetRef->SeparateWrapMode)
        	{
        		SamplerState::WrapMode wrapMode;
        		assetRef->SamplerState->GetWrapMode(wrapMode, wrapMode, wrapMode);

        		bool changed = false;
        		changed |= EditorGUI::DrawComboBox("Wrap Mode", stringValues, wrapMode);

        		if (changed)
        			assetRef->SamplerState->SetWrapMode(wrapMode, wrapMode, wrapMode);
        	}
	        else
	        {
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
	    }

	    static void DrawMipMaps(const Ref<TextureAsset>& assetRef)
	    {
		    if (EditorGUI::DrawCheckBox("Generate Mip Maps", assetRef->GenerateMipMaps))
			    assetRef->CreateTexture();
        	
		    if (!assetRef->GenerateMipMaps) return;

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
		    
	    }

	    static void DrawCompare(const Ref<TextureAsset>& assetRef)
	    {
		    {
			    const Array<String, 2> stringValues{"None", "CompareRefToTexture" };

			    SamplerState::CompareMode compareMode = assetRef->SamplerState->GetCompareMode();

			    if (EditorGUI::DrawComboBox("Compare Mode", stringValues, compareMode))
				    assetRef->SamplerState->SetCompareMode(compareMode);
		    }

        	if (assetRef->SamplerState->GetCompareMode() == SamplerState::CompareMode::None) return;
        	
		    {
			    const Array<String, 8> stringValues{"Never", "Less",
				    "Equal", "LessOrEqual",
				    "Greater", "NotEqual",
				    "GreaterOrEqual", "Always" };

			    SamplerState::CompareFunc compareMode = assetRef->SamplerState->GetCompareFunction();

			    if (EditorGUI::DrawComboBox("Compare Function", stringValues, compareMode))
				    assetRef->SamplerState->SetCompareFunction(compareMode);
		    }
	    }

	    static void DrawFiltering(const Ref<TextureAsset>& assetRef)
	    {
		    EditorGUI::DrawCheckBox("Enable Anisotropic Filtering", 
		                            BindGetter(&SamplerState::IsAnisotropicFilteringEnabled, assetRef->SamplerState.get()),
		                            BindSetter(&SamplerState::EnableAnisotropicFiltering, assetRef->SamplerState.get()));

		    if (!assetRef->SamplerState->IsAnisotropicFilteringEnabled()) return;

        	EditorGUI::DrawIntControl("Max Anisotropy",
		                              BindGetter(&SamplerState::GetMaxAnisotropy, assetRef->SamplerState.get()),
		                              BindSetter(&SamplerState::SetMaxAnisotropy, assetRef->SamplerState.get()),
		                              1, 1, 16);
	    }
    };
}
