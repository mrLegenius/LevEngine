#include "Texture.h"

#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLTexture.h"
#include "Platforms/OpenGL/OpenGLTextureSkybox.h"

namespace LevEngine
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(width, height);
		}

		LEV_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI()) {

		case RendererAPI::API::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(path);
		}

		LEV_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

    Ref<TextureSkybox> TextureSkybox::Create(std::string paths[6])
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:
            LEV_CORE_ASSERT(false, "None for API was chosen");
                return nullptr;
            case RendererAPI::API::OpenGL:
                return CreateRef<OpenGLTextureSkybox>(paths);
        }

        LEV_CORE_ASSERT(false, "Unknown Renderer API");
        return nullptr;
    }
}
