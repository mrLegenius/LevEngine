#pragma once

#include <glad/gl.h>
#include "Renderer/Texture.h"

namespace LevEngine
{
    class OpenGLTextureSkybox : public TextureSkybox
    {
    public:
        explicit OpenGLTextureSkybox(std::string paths[6]);

        ~OpenGLTextureSkybox() override;

        [[nodiscard]] uint32_t GetWidth() const override
        { return m_Width; }

        [[nodiscard]] uint32_t GetHeight() const override
        { return m_Height; }

        [[nodiscard]] uint32_t GetRendererID() const override
        { return m_RendererID; }

        [[nodiscard]] const std::string &GetPath(int index) const override
        { return m_Paths[index]; }

        void SetData(void *data, uint32_t size) override;

        void Bind(uint32_t slot = 0) const override;

        [[nodiscard]] bool IsLoaded() const override
        {
            return m_IsLoaded;
        }

        bool operator==(const Texture &other) const override
        {
            return m_RendererID == other.GetRendererID();
        }

    private:
        std::string m_Paths[6];
        bool m_IsLoaded = false;
        uint32_t m_RendererID;

        uint32_t m_Width;
        uint32_t m_Height;
    };
}