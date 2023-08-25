#pragma once
#include <imgui.h>

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
			m_SamplerState = SamplerState::Create();
			m_Texture->AttachSampler(m_SamplerState);
		}

		[[nodiscard]] const Ref<Texture>& GetTexture() const { return m_Texture; }

		void DrawProperties() override;

	protected:
		void SerializeMeta(YAML::Emitter& out) override { }
		bool DeserializeMeta(YAML::Node& out) override
		{

			return true;
		}

		void SerializeData(YAML::Emitter& out) override { }
		bool DeserializeData(YAML::Node& node) override { return true; }
		
	private:
		Ref<Texture> m_Texture;
		Ref<SamplerState> m_SamplerState;
	};
}
