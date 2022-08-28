#include "OpenGLTexture.h"

#include "stb_image.h"

namespace LevEngine
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path), m_InternalFormat(0), m_DataFormat(0)
	{
		LEV_PROFILE_FUNCTION();
		
		int width, height, channels;
		
		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data;
		{
			LEV_PROFILE_SCOPE("stbi texture file loading - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		
        if (!data) return;

        m_IsLoaded = true;
		m_Width = width;
		m_Height = height;

		if (channels == 3)
		{
			m_InternalFormat = GL_RGB8;
			m_DataFormat = GL_RGB;
		}
		else if(channels == 4)
		{
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
		}

		LEV_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Format is not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(const uint32_t width, const uint32_t height)
		: m_Width(width), m_Height(height), m_InternalFormat(GL_RGBA8), m_DataFormat(GL_RGBA)
	{
		LEV_PROFILE_FUNCTION();
		
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		LEV_PROFILE_FUNCTION();
		
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, const uint32_t size)
	{
		LEV_PROFILE_FUNCTION();
		
		const uint32_t bpc = m_DataFormat == GL_RGBA ? 4 : 3;
		LEV_CORE_ASSERT(size == m_Width * m_Height * bpc, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(const uint32_t slot) const
	{
		LEV_PROFILE_FUNCTION();
		
		glBindTextureUnit(slot, m_RendererID);
	}
}
