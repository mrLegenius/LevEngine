#include "levpch.h"
#include "VertexBuffer.h"

#include "RenderSettings.h"
#include "Platform/D3D11/D3D11VertexBuffer.h"

namespace LevEngine
{
	Ref<VertexBuffer> VertexBuffer::Create(const uint32_t size, const uint32_t stride)
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11VertexBuffer>(size, stride);
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}

	Ref<VertexBuffer> VertexBuffer::Create(const float* vertices, const uint32_t size, const uint32_t stride)
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11VertexBuffer>(vertices, size, stride);
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}

	Ref<VertexBuffer> VertexBuffer::Create(const int* vertices, const uint32_t size, const uint32_t stride)
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11VertexBuffer>(vertices, size, stride);
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}
}
