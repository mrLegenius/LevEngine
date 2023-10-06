#pragma once
#include "Asset.h"
#include "TextureLibrary.h"
#include "Renderer/Texture.h"

namespace LevEngine
{
	class TextureAsset final : public Asset
	{
	public:
		Ref<SamplerState> samplerState;
		
		TextureAsset(const Path& path, const UUID& uuid) : Asset(path, uuid)
		{
			samplerState = SamplerState::Create();
		}

		[[nodiscard]] const Ref<Texture>& GetTexture() const { return m_Texture; }
		
		[[nodiscard]] Ref<Texture> GetIcon() const override
		{
			if (m_Texture)
				return m_Texture;

			return Asset::GetIcon();
		}
		
	protected:
		[[nodiscard]] bool WriteDataToFile() const override { return false; }
		[[nodiscard]] bool ReadDataFromFile() const override { return false; }

		void SerializeMeta(YAML::Emitter& out) override;
		void DeserializeMeta(YAML::Node& out) override;

		void SerializeData(YAML::Emitter& out) override { }
		void DeserializeData(YAML::Node& node) override
		{
			m_Texture = TextureLibrary::GetTexture(m_Path.string().c_str());
			m_Texture->AttachSampler(samplerState);
		}
		
	private:
		Ref<Texture> m_Texture;
	};
}
