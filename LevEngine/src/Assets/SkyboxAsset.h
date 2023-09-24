#pragma once
#include "Asset.h"
#include "TextureAsset.h"
#include "Renderer/Texture.h"

namespace LevEngine
{
	class SkyboxAsset final : public Asset
	{
	public:
		Ref<TextureAsset> left;
		Ref<TextureAsset> right;
		Ref<TextureAsset> bottom;
		Ref<TextureAsset> top;
		Ref<TextureAsset> back;
		Ref<TextureAsset> front;
		
		enum class Side
		{
			Left,
			Right,
			Bottom,
			Top,
			Back,
			Front
		};

		explicit SkyboxAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }

		[[nodiscard]] const Ref<Texture>& GetTexture() const { return m_Texture; }

		[[nodiscard]] Ref<Texture> GetIcon() const override
		{
			return Icons::Skybox();
		}
		
	protected:
		void SerializeData(YAML::Emitter& out) override;
		void DeserializeData(YAML::Node& node) override;
	private:
		void InitTexture();

		Ref<Texture> m_Texture;
	};
}

