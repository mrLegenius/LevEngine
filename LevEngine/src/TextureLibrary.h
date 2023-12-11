#pragma once

namespace LevEngine
{
	class Texture;

	class TextureLibrary
	{
	public:
		static Ref<Texture> GetWhiteTexture();
		static Ref<Texture> GetBlackTexture();
		static Ref<Texture> GetEmptyNormalMap();
		static Ref<Texture> GetTexture(const String& path);

	private:
		static inline UnorderedMap<String, Ref<Texture>> m_TextureMap;
	};
}
