#pragma once
#include "Asset.h"
#include "TextureLibrary.h"
#include "Renderer/Texture.h"

namespace LevEngine
{
	class TextureAsset final : public Asset
	{
	public:
		TextureAsset(const std::filesystem::path& path, const UUID& uuid) : Asset(path, uuid)
		{
			m_Texture = TextureLibrary::GetTexture(path.string());
		}

		[[nodiscard]] const Ref<Texture>& GetTexture() const { return m_Texture; }

		void DrawProperties() override { }

	protected:
		void SerializeData(YAML::Emitter& out) override { }
		bool DeserializeData(YAML::Node& node) override { return true; }
		
	private:
		Ref<Texture> m_Texture;
	};
}
