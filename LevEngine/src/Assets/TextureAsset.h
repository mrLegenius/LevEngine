#pragma once
#include "Asset.h"

namespace LevEngine
{
	class SamplerState;
	class Texture;
	class TextureAsset final : public Asset
	{
	public:
		Ref<SamplerState> SamplerState;
		bool IsLinear = false;
		bool GenerateMipMaps = false;

		TextureAsset(const Path& path, const UUID& uuid);

		[[nodiscard]] const Ref<Texture>& GetTexture() const { return m_Texture; }
		
		[[nodiscard]] Ref<Texture> GetIcon() const override
		{
			if (m_Texture)
				return m_Texture;

			return Asset::GetIcon();
		}

		void CreateTexture();

	protected:
		[[nodiscard]] bool WriteDataToFile() const override { return false; }
		[[nodiscard]] bool ReadDataFromFile() const override { return false; }

		void SerializeMeta(YAML::Emitter& out) override;
		void DeserializeMeta(const YAML::Node& out) override;

		void SerializeData(YAML::Emitter& out) override { }
		void DeserializeData(const YAML::Node& node) override { CreateTexture(); }
		
	private:
		Ref<Texture> m_Texture;
	};
}
