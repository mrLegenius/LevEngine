#pragma once
#include "DataTypes/UnorderedMap.h"
#include "Renderer/Texture.h"

namespace LevEngine
{
	class TextureLibrary
	{
	public:
		static Ref<Texture> GetWhiteTexture()
		{
			static uint8_t data[3] = {255, 255, 255};
			static Ref<Texture> emptyTexture = Texture::CreateTexture2D(1, 1, 1, Texture::TextureFormat(), &data[0]);
			return emptyTexture;
		}
		static Ref<Texture> GetBlackTexture()
		{
			static uint8_t data[3] = {0, 0, 0};
			static Ref<Texture> emptyTexture = Texture::CreateTexture2D(1, 1, 1, Texture::TextureFormat(), &data[0]);
			return emptyTexture;
		}

		static Ref<Texture> GetEmptyNormalMap()
		{
			static uint8_t data[3] = {128, 128, 255};
			static Ref<Texture> emptyTexture = Texture::CreateTexture2D(1, 1, 1, Texture::TextureFormat(), &data[0]);
			return emptyTexture;
		}
		
		static Ref<Texture> GetTexture(const String& path)
		{
			if (const auto texture = m_TextureMap.find(path); texture != m_TextureMap.end())
				return texture->second;

			auto newTexture = Texture::Create(path);
			m_TextureMap.emplace(path, newTexture);
			return newTexture;
		}

	private:
		static inline UnorderedMap<String, Ref<Texture>> m_TextureMap;
	};
}
