#pragma once
#include "DataTypes/UnorderedMap.h"
#include "Renderer/Texture.h"

namespace LevEngine
{
	class TextureLibrary
	{
	public:
		static const Ref<Texture>& GetTexture(const std::string& path)
		{
			if (const auto texture = m_TextureMap.find(path); texture != m_TextureMap.end())
				return texture->second;

			auto newTexture = Texture::Create(path);
			m_TextureMap.emplace(path, newTexture);
			return newTexture;
		}

	private:
		static inline UnorderedMap<std::string, Ref<Texture>> m_TextureMap;
	};
}
