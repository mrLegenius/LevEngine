#include "levpch.h"
#include "TextureAsset.h"

namespace LevEngine
{
	void TextureAsset::SerializeMeta(YAML::Emitter& out)
	{
		out << YAML::Key << "IsLinear" << YAML::Value << IsLinear;
		
		SamplerState::MinFilter minFilter;
		SamplerState::MagFilter magFilter;
		SamplerState::MipFilter mipFilter;
		SamplerState->GetFilter(minFilter, magFilter, mipFilter);
		out << YAML::Key << "MinFilter" << YAML::Value << static_cast<int>(minFilter);
		out << YAML::Key << "MagFilter" << YAML::Value << static_cast<int>(magFilter);
		out << YAML::Key << "MipFilter" << YAML::Value << static_cast<int>(mipFilter);

		SamplerState::WrapMode wrapModeU;
		SamplerState::WrapMode wrapModeV;
		SamplerState::WrapMode wrapModeW;
		SamplerState->GetWrapMode(wrapModeU, wrapModeV, wrapModeW);
		out << YAML::Key << "WrapModeU" << YAML::Value << static_cast<int>(wrapModeU);
		out << YAML::Key << "WrapModeV" << YAML::Value << static_cast<int>(wrapModeV);
		out << YAML::Key << "WrapModeW" << YAML::Value << static_cast<int>(wrapModeW);

		const SamplerState::CompareMode compareMode = SamplerState->GetCompareMode();
		out << YAML::Key << "CompareMode" << YAML::Value << static_cast<int>(compareMode);

		const SamplerState::CompareFunc compareFunc = SamplerState->GetCompareFunction();
		out << YAML::Key << "CompareFunc" << YAML::Value << static_cast<int>(compareFunc);

		const float LODBias = SamplerState->GetLODBias();
		out << YAML::Key << "LODBias" << YAML::Value << LODBias;

		const float minLOD = SamplerState->GetMinLOD();
		out << YAML::Key << "MinLOD" << YAML::Value << minLOD;

		const float maxLOD = SamplerState->GetMaxLOD();
		out << YAML::Key << "MaxLOD" << YAML::Value << maxLOD;

		const int maxAnisotropy = SamplerState->GetMaxAnisotropy();
		out << YAML::Key << "MaxAnisotropy" << YAML::Value << maxAnisotropy;

		const bool isAnisotropicFilteringEnabled = SamplerState->IsAnisotropicFilteringEnabled();
		out << YAML::Key << "IsAnisotropicFilteringEnabled" << YAML::Value << isAnisotropicFilteringEnabled;
	}

	void TextureAsset::DeserializeMeta(YAML::Node& out)
	{
		if (const auto linear = out["IsLinear"])
			IsLinear = linear.as<bool>();
		
		const auto minFilter = static_cast<SamplerState::MinFilter>(out["MinFilter"].as<int>());
		const auto magFilter = static_cast<SamplerState::MagFilter>(out["MagFilter"].as<int>());
		const auto mipFilter = static_cast<SamplerState::MipFilter>(out["MipFilter"].as<int>());
		SamplerState->SetFilter(minFilter, magFilter, mipFilter);

		const auto wrapModeU = static_cast<SamplerState::WrapMode>(out["WrapModeU"].as<int>());
		const auto wrapModeV = static_cast<SamplerState::WrapMode>(out["WrapModeV"].as<int>());
		const auto wrapModeW = static_cast<SamplerState::WrapMode>(out["WrapModeW"].as<int>());
		SamplerState->SetWrapMode(wrapModeU, wrapModeV, wrapModeW);

		const auto compareMode = static_cast<SamplerState::CompareMode>(out["CompareMode"].as<int>());
		SamplerState->SetCompareMode(compareMode);

		const auto compareFunc = static_cast<SamplerState::CompareFunc>(out["CompareFunc"].as<int>());
		SamplerState->SetCompareFunction(compareFunc);

		SamplerState->SetLODBias(out["LODBias"].as<float>());
		SamplerState->SetMinLOD(out["MinLOD"].as<float>());
		SamplerState->SetMaxLOD(out["MaxLOD"].as<float>());
		SamplerState->SetMaxAnisotropy(out["MaxAnisotropy"].as<int>());
		SamplerState->SetMaxAnisotropy(out["IsAnisotropicFilteringEnabled"].as<bool>());
	}
}
