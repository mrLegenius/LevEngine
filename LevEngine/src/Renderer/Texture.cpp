#include "pch.h"
#include "Texture.h"

#include "RenderSettings.h"
#include "Platform/D3D11/D3D11Texture.h"

namespace LevEngine
{
	Ref<Texture> Texture::Create(const std::string& path)
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_THROW("None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11Texture>(path);
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}

	
	Ref<Texture> Texture::CreateTexture2D(const uint16_t width, const uint16_t height, const uint16_t slices, const TextureFormat& format, const CPUAccess cpuAccess, const bool uav)
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_THROW("None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return D3D11Texture::CreateTexture2D(width, height, slices, format, cpuAccess, uav);
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}

	Ref<Texture> Texture::CreateTextureCube(const std::string paths[6])
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_THROW("None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11Texture>(paths);
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}
}
