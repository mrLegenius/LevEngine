#include "levpch.h"
#include "TextureAsset.h"

namespace LevEngine
{
	void TextureAsset::SerializeMeta(YAML::Emitter& out)
	{
		SamplerState::MinFilter minFilter;
		SamplerState::MagFilter magFilter;
		SamplerState::MipFilter mipFilter;
		samplerState->GetFilter(minFilter, magFilter, mipFilter);
		out << YAML::Key << "MinFilter" << YAML::Value << static_cast<int>(minFilter);
		out << YAML::Key << "MagFilter" << YAML::Value << static_cast<int>(magFilter);
		out << YAML::Key << "MipFilter" << YAML::Value << static_cast<int>(mipFilter);

		SamplerState::WrapMode wrapModeU;
		SamplerState::WrapMode wrapModeV;
		SamplerState::WrapMode wrapModeW;
		samplerState->GetWrapMode(wrapModeU, wrapModeV, wrapModeW);
		out << YAML::Key << "WrapModeU" << YAML::Value << static_cast<int>(wrapModeU);
		out << YAML::Key << "WrapModeV" << YAML::Value << static_cast<int>(wrapModeV);
		out << YAML::Key << "WrapModeW" << YAML::Value << static_cast<int>(wrapModeW);

		const SamplerState::CompareMode compareMode = samplerState->GetCompareMode();
		out << YAML::Key << "CompareMode" << YAML::Value << static_cast<int>(compareMode);

		const SamplerState::CompareFunc compareFunc = samplerState->GetCompareFunction();
		out << YAML::Key << "CompareFunc" << YAML::Value << static_cast<int>(compareFunc);

		const float LODBias = samplerState->GetLODBias();
		out << YAML::Key << "LODBias" << YAML::Value << LODBias;

		const float minLOD = samplerState->GetMinLOD();
		out << YAML::Key << "MinLOD" << YAML::Value << minLOD;

		const float maxLOD = samplerState->GetMaxLOD();
		out << YAML::Key << "MaxLOD" << YAML::Value << maxLOD;

		const int maxAnisotropy = samplerState->GetMaxAnisotropy();
		out << YAML::Key << "MaxAnisotropy" << YAML::Value << maxAnisotropy;

		const bool isAnisotropicFilteringEnabled = samplerState->IsAnisotropicFilteringEnabled();
		out << YAML::Key << "IsAnisotropicFilteringEnabled" << YAML::Value << isAnisotropicFilteringEnabled;
	}

	void TextureAsset::DeserializeMeta(YAML::Node& out)
	{
		const auto minFilter = static_cast<SamplerState::MinFilter>(out["MinFilter"].as<int>());
		const auto magFilter = static_cast<SamplerState::MagFilter>(out["MagFilter"].as<int>());
		const auto mipFilter = static_cast<SamplerState::MipFilter>(out["MipFilter"].as<int>());
		samplerState->SetFilter(minFilter, magFilter, mipFilter);

		const auto wrapModeU = static_cast<SamplerState::WrapMode>(out["WrapModeU"].as<int>());
		const auto wrapModeV = static_cast<SamplerState::WrapMode>(out["WrapModeV"].as<int>());
		const auto wrapModeW = static_cast<SamplerState::WrapMode>(out["WrapModeW"].as<int>());
		samplerState->SetWrapMode(wrapModeU, wrapModeV, wrapModeW);

		const auto compareMode = static_cast<SamplerState::CompareMode>(out["CompareMode"].as<int>());
		samplerState->SetCompareMode(compareMode);

		const auto compareFunc = static_cast<SamplerState::CompareFunc>(out["CompareFunc"].as<int>());
		samplerState->SetCompareFunction(compareFunc);

		samplerState->SetLODBias(out["LODBias"].as<float>());
		samplerState->SetMinLOD(out["MinLOD"].as<float>());
		samplerState->SetMaxLOD(out["MaxLOD"].as<float>());
		samplerState->SetMaxAnisotropy(out["MaxAnisotropy"].as<int>());
		samplerState->SetMaxAnisotropy(out["IsAnisotropicFilteringEnabled"].as<bool>());
	}
}
