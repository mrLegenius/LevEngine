#pragma once
#include "Asset.h"

namespace LevEngine
{
	class Texture;
	class TextureAsset;

	class SkyboxAsset final : public Asset
	{
	public:
		Ref<TextureAsset> HDRTexture;

		explicit SkyboxAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }

		[[nodiscard]] const Ref<Texture>& GetTexture() const;
		[[nodiscard]] Ref<Texture> GetIcon() const override { return Icons::Skybox(); }
		
	protected:
		void SerializeData(YAML::Emitter& out) override;
		void DeserializeData(YAML::Node& node) override;
	};
}

