#include "Framebuffer.h"

#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLFramebuffer.h"

namespace LevEngine
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLFramebuffer>(spec);
		}

		LEV_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}
