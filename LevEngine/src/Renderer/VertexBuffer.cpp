#include "pch.h"
#include "VertexBuffer.h"

#include "RenderSettings.h"
#include "Platform/D3D11/D3D11VertexBuffer.h"

namespace LevEngine
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11VertexBuffer>(size);
		default:
			LEV_THROW("Unknown Renderer API")
		}
	}

	Ref<VertexBuffer> VertexBuffer::Create(const float* vertices, uint32_t size)
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11VertexBuffer>(vertices, size);
		default:
			LEV_THROW("Unknown Renderer API")
		}
	}
}
