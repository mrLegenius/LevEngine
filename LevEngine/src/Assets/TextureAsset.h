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
			m_SamplerState = SamplerState::Create();
		}

		[[nodiscard]] const Ref<Texture>& GetTexture() const { return m_Texture; }

		void DrawProperties() override;

	protected:
		[[nodiscard]] bool DoSerializeData() const override { return false; }

		void SerializeMeta(YAML::Emitter& out) override;
		void DeserializeMeta(YAML::Node& out) override;

		void SerializeData(YAML::Emitter& out) override { }
		void DeserializeData(YAML::Node& node) override
		{
			m_Texture = TextureLibrary::GetTexture(m_Path.string());
			m_Texture->AttachSampler(m_SamplerState);
		}
		
	private:
		Ref<Texture> m_Texture;
		Ref<SamplerState> m_SamplerState;
	};
}
