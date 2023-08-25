#include "levpch.h"
#include "TextureAsset.h"

#include "GUI/GUIUtils.h"

namespace LevEngine
{
	void TextureAsset::DrawProperties()
	{
		{
			const char* filterStrings[] = { "Nearest", "Linear" };
			SamplerState::MinFilter minFilter;
			SamplerState::MagFilter magFilter;
			SamplerState::MipFilter mipFilter;
			m_SamplerState->GetFilter(minFilter, magFilter, mipFilter);
			bool changed = false;
			changed |= GUIUtils::DrawComboBox("Min Filter", filterStrings, 2, &minFilter);
			changed |= GUIUtils::DrawComboBox("Mag Filter", filterStrings, 2, &magFilter);
			changed |= GUIUtils::DrawComboBox("Mip Filter", filterStrings, 2, &mipFilter);

			if (changed)
				m_SamplerState->SetFilter(minFilter, magFilter, mipFilter);
		}

		{
			const char* wrapStrings[] = { "Repeat", "Mirror", "Clamp", "Border" };
			SamplerState::WrapMode wrapModeU;
			SamplerState::WrapMode wrapModeV;
			SamplerState::WrapMode wrapModeW;
			m_SamplerState->GetWrapMode(wrapModeU, wrapModeV, wrapModeW);

			bool changed = false;
			changed |= GUIUtils::DrawComboBox("Wrap Mode U", wrapStrings, 4, &wrapModeU);
			changed |= GUIUtils::DrawComboBox("Wrap Mode V", wrapStrings, 4, &wrapModeV);
			changed |= GUIUtils::DrawComboBox("Wrap Mode W", wrapStrings, 4, &wrapModeW);

			if (changed)
				m_SamplerState->SetWrapMode(wrapModeU, wrapModeV, wrapModeW);
		}
			
		{
			const char* compareModeStrings[] = { "None", "CompareRefToTexture" };

			SamplerState::CompareMode compareMode = m_SamplerState->GetCompareMode();

			bool changed = GUIUtils::DrawComboBox("Compare Mode", compareModeStrings, 2, &compareMode);

			if (changed)
				m_SamplerState->SetCompareMode(compareMode);
		}

		{
			const char* compareModeStrings[] = { "Never", "Less", "Equal", "LessOrEqual", "Greater", "NotEqual", "GreaterOrEqual", "Always" };

			SamplerState::CompareFunc compareMode = m_SamplerState->GetCompareFunction();

			bool changed = GUIUtils::DrawComboBox("Compare Function", compareModeStrings, 8, &compareMode);

			if (changed)
				m_SamplerState->SetCompareFunction(compareMode);
		}

		GUIUtils::DrawFloatControl("LOD Bias",
			BindGetter(&SamplerState::GetLODBias, m_SamplerState.get()),
			BindSetter(&SamplerState::SetLODBias, m_SamplerState.get()),
			1.0f, 0.0f, std::numeric_limits<float>::max());

		GUIUtils::DrawFloatControl("Min LOD",
			BindGetter(&SamplerState::GetMinLOD, m_SamplerState.get()),
			BindSetter(&SamplerState::SetMinLOD, m_SamplerState.get()),
			1.0f, 0.0f, std::numeric_limits<float>::max());

		GUIUtils::DrawFloatControl("Max LOD",
			BindGetter(&SamplerState::GetMaxLOD, m_SamplerState.get()),
			BindSetter(&SamplerState::SetMaxLOD, m_SamplerState.get()), 
			1.0f, 0.0f, std::numeric_limits<float>::max());

		GUIUtils::DrawColor3Control("Border Color", 
			BindGetter(&SamplerState::GetBorderColor, m_SamplerState.get()),
			BindSetter(&SamplerState::SetBorderColor, m_SamplerState.get()));

		GUIUtils::DrawCheckBox("Enable Anisotropic Filtering", 
			BindGetter(&SamplerState::IsAnisotropicFilteringEnabled, m_SamplerState.get()),
			BindSetter(&SamplerState::EnableAnisotropicFiltering, m_SamplerState.get()));

		GUIUtils::DrawIntControl("Max Anisotropy",
			BindGetter(&SamplerState::GetMaxAnisotropy, m_SamplerState.get()),
			BindSetter(&SamplerState::SetMaxAnisotropy, m_SamplerState.get()), 
			1, 1, 16);
	}
}
