#include "levpch.h"
#include "IndexBuffer.h"

#include "Platform/D3D11/D3D11IndexBuffer.h"
#include "Renderer/RenderSettings.h"

namespace LevEngine
{
	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count)
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11IndexBuffer>(indices, count);
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}

		return nullptr;
	}
}
