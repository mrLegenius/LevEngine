#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
	class Texture;

	class LEV_API TextureLibrary
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
