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

	void TextureAsset::SerializeMeta(YAML::Emitter& out)
	{
		SamplerState::MinFilter minFilter;
		SamplerState::MagFilter magFilter;
		SamplerState::MipFilter mipFilter;
		m_SamplerState->GetFilter(minFilter, magFilter, mipFilter);
		out << YAML::Key << "MinFilter" << YAML::Value << static_cast<int>(minFilter);
		out << YAML::Key << "MagFilter" << YAML::Value << static_cast<int>(magFilter);
		out << YAML::Key << "MipFilter" << YAML::Value << static_cast<int>(mipFilter);

		SamplerState::WrapMode wrapModeU;
		SamplerState::WrapMode wrapModeV;
		SamplerState::WrapMode wrapModeW;
		m_SamplerState->GetWrapMode(wrapModeU, wrapModeV, wrapModeW);
		out << YAML::Key << "WrapModeU" << YAML::Value << static_cast<int>(wrapModeU);
		out << YAML::Key << "WrapModeV" << YAML::Value << static_cast<int>(wrapModeV);
		out << YAML::Key << "WrapModeW" << YAML::Value << static_cast<int>(wrapModeW);

		const SamplerState::CompareMode compareMode = m_SamplerState->GetCompareMode();
		out << YAML::Key << "CompareMode" << YAML::Value << static_cast<int>(compareMode);

		const SamplerState::CompareFunc compareFunc = m_SamplerState->GetCompareFunction();
		out << YAML::Key << "CompareFunc" << YAML::Value << static_cast<int>(compareFunc);

		const float LODBias = m_SamplerState->GetLODBias();
		out << YAML::Key << "LODBias" << YAML::Value << LODBias;

		const float minLOD = m_SamplerState->GetMinLOD();
		out << YAML::Key << "MinLOD" << YAML::Value << minLOD;

		const float maxLOD = m_SamplerState->GetMaxLOD();
		out << YAML::Key << "MaxLOD" << YAML::Value << maxLOD;

		const int maxAnisotropy = m_SamplerState->GetMaxAnisotropy();
		out << YAML::Key << "MaxAnisotropy" << YAML::Value << maxAnisotropy;

		const bool isAnisotropicFilteringEnabled = m_SamplerState->IsAnisotropicFilteringEnabled();
		out << YAML::Key << "IsAnisotropicFilteringEnabled" << YAML::Value << isAnisotropicFilteringEnabled;
	}

	void TextureAsset::DeserializeMeta(YAML::Node& out)
	{
		const auto minFilter = static_cast<SamplerState::MinFilter>(out["MinFilter"].as<int>());
		const auto magFilter = static_cast<SamplerState::MagFilter>(out["MagFilter"].as<int>());
		const auto mipFilter = static_cast<SamplerState::MipFilter>(out["MipFilter"].as<int>());
		m_SamplerState->SetFilter(minFilter, magFilter, mipFilter);

		const auto wrapModeU = static_cast<SamplerState::WrapMode>(out["WrapModeU"].as<int>());
		const auto wrapModeV = static_cast<SamplerState::WrapMode>(out["WrapModeV"].as<int>());
		const auto wrapModeW = static_cast<SamplerState::WrapMode>(out["WrapModeW"].as<int>());
		m_SamplerState->SetWrapMode(wrapModeU, wrapModeV, wrapModeW);

		const auto compareMode = static_cast<SamplerState::CompareMode>(out["CompareMode"].as<int>());
		m_SamplerState->SetCompareMode(compareMode);

		const auto compareFunc = static_cast<SamplerState::CompareFunc>(out["CompareFunc"].as<int>());
		m_SamplerState->SetCompareFunction(compareFunc);

		m_SamplerState->SetLODBias(out["LODBias"].as<float>());
		m_SamplerState->SetMinLOD(out["MinLOD"].as<float>());
		m_SamplerState->SetMaxLOD(out["MaxLOD"].as<float>());
		m_SamplerState->SetMaxAnisotropy(out["MaxAnisotropy"].as<int>());
		m_SamplerState->SetMaxAnisotropy(out["IsAnisotropicFilteringEnabled"].as<bool>());
	}
}
