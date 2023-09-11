#pragma once
#include "Asset.h"
#include "TextureAsset.h"
#include "Renderer/Texture.h"

namespace LevEngine
{
	class SkyboxAsset final : public Asset
	{
	public:
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

		void DrawProperties() override;

		[[nodiscard]] const Ref<Texture>& GetTexture() const { return m_Texture; }

	protected:
		void SerializeData(YAML::Emitter& out) override;
		void DeserializeData(YAML::Node& node) override;
	private:
		void InitTexture();

		Ref<Texture> m_Texture;

		Ref<TextureAsset> m_Left;
		Ref<TextureAsset> m_Right;
		Ref<TextureAsset> m_Bottom;
		Ref<TextureAsset> m_Top;
		Ref<TextureAsset> m_Back;
		Ref<TextureAsset> m_Front;
	};
}

