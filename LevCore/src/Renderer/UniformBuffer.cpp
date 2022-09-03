#include "UniformBuffer.h"

#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLUniformBuffer.h"

namespace LevEngine
{
    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:
                LEV_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                return nullptr;
            case RendererAPI::API::OpenGL:
                return CreateRef<OpenGLUniformBuffer>(size, binding);
        }

        LEV_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
