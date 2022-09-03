#include "StorageBuffer.h"

#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLStorageBuffer.h"

namespace LevEngine
{
    Ref<StorageBuffer> StorageBuffer::Create(uint32_t binding)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:
            LEV_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                return nullptr;
            case RendererAPI::API::OpenGL:
                return CreateRef<OpenGLStorageBuffer>(binding);
        }

        LEV_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}