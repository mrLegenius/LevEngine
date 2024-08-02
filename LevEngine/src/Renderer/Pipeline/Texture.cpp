#include "levpch.h"
#include "Texture.h"

#include "Kernel/Application.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
	Ref<Texture> Texture::Create(const String& path)
	{
		return Create(path, true, true);
	}

	Ref<Texture> Texture::Create(const String& path,
		bool isLinear,
		bool generateMipMaps)
	{
		return App::RenderDevice().CreateTexture(path, isLinear, generateMipMaps);
	}

	Ref<Texture> Texture::CreateTexture2D(
		const uint16_t width, const uint16_t height, const uint16_t slices,
		const TextureFormat& format,
		void* data,
		const CPUAccess cpuAccess,
		const bool uav,
		const bool generateMipMaps)
	{
		return App::RenderDevice().CreateTexture2D(width, height, slices, format, data, cpuAccess, uav, generateMipMaps);
	}

	Ref<Texture> Texture::CreateTexture2D(
		const uint16_t width, const uint16_t height, const uint16_t slices,
		const TextureFormat& format,
		const CPUAccess cpuAccess,
		const bool uav,
		const bool generateMipMaps)
	{
		return App::RenderDevice().CreateTexture2D(width, height, slices, format, cpuAccess, uav, generateMipMaps);
	}

	Ref<Texture> Texture::CreateTextureCube(
		const uint16_t width, const uint16_t height,
		const TextureFormat& format,
		const CPUAccess cpuAccess,
		const bool uav,
		const bool generateMipMaps)
	{
		return App::RenderDevice().CreateTextureCube(width, height, format, cpuAccess, uav, generateMipMaps);
	}
	
	Ref<Texture> Texture::CreateTextureCube(const String paths[6])
	{
		return CreateTextureCube(paths, false);
	}

	Ref<Texture> Texture::CreateTextureCube(const String paths[6], const bool isLinear)
	{
		return App::RenderDevice().CreateTextureCube(paths, isLinear);
	}
}
