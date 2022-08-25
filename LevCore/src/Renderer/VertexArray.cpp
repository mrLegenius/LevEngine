#include "VertexArray.h"

#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLVertexArray.h"

namespace LevEngine
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexArray>();
		}

		LEV_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}
