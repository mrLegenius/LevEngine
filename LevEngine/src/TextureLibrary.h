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

		//Releases every cached texture. Has to be called while the render device is still alive:
		//textures left in the cache would be destroyed during CRT teardown instead, and releasing
		//D3D11 objects at that point deadlocks inside the graphics driver
		static void Shutdown();

	private:
		static inline UnorderedMap<String, Ref<Texture>> m_TextureMap;

		//<--- Kept as members instead of function local statics, so Shutdown can release them ---<<
		static inline Ref<Texture> m_WhiteTexture;
		static inline Ref<Texture> m_BlackTexture;
		static inline Ref<Texture> m_EmptyNormalMap;
	};
}
