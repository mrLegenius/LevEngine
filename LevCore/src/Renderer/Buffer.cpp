#include "Buffer.h"
#include "Renderer.h"
#include "../Platforms/OpenGL/OpenGLBuffer.h"

namespace LevEngine
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
			return nullptr;
		case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLVertexBuffer>(size);
		}

		LEV_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, const uint32_t size)
	{
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
			return nullptr;
		case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		LEV_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, const uint32_t count)
	{
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:
			Log::CoreCritical("None of Renderer API was chosen");
			LEV_CORE_ASSERT(false, "None for API was chosen");
			return nullptr;
		case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLIndexBuffer>(indices, count);
		}

		LEV_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}
