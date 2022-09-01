#include "OpenGLTextureSkybox.h"

#include "stb_image.h"

namespace LevEngine
{
    OpenGLTextureSkybox::OpenGLTextureSkybox(std::string paths[6])
        : m_Width(0), m_Height(0)
    {
        LEV_PROFILE_FUNCTION();

        std::copy(paths, paths + 6, m_Paths);

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

        int width, height, channels;
        stbi_set_flip_vertically_on_load(0);

        for (unsigned int i = 0; i < 6; i++)
        {
            auto path = paths[i];
            stbi_uc* data;
            {
                LEV_PROFILE_SCOPE("stbi texture file loading - OpenGLTextureSkybox::OpenGLTextureSkybox(std::string paths[6])");
                data = stbi_load(path.c_str(), &width, &height, &channels, 0);
            }

            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                m_Width += width;
                m_Height += height;
                stbi_image_free(data);
            }
            else
            {
                Log::CoreError("Cube map texture failed to load at path: {0}", path);
                glDeleteTextures(1, &m_RendererID);
                return;
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        m_IsLoaded = true;
    }

    OpenGLTextureSkybox::~OpenGLTextureSkybox()
    {
        LEV_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTextureSkybox::SetData(void *data, const uint32_t size)
    {
        LEV_PROFILE_FUNCTION();

        LEV_NOT_IMPLEMENTED
    }

    void OpenGLTextureSkybox::Bind(const uint32_t slot) const
    {
        LEV_PROFILE_FUNCTION();

        glBindTextureUnit(slot, m_RendererID);
    }
}