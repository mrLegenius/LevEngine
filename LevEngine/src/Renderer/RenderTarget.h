#pragma once
#include "ClearFlags.h"
#include "DataTypes/Vector.h"
#include "Math/Vector4.h"

namespace LevEngine
{
    class Texture;

    enum class AttachmentPoint : uint8_t
    {
        Color0,         // Must be a uncompressed color format.
        Color1,         // Must be a uncompressed color format.
        Color2,         // Must be a uncompressed color format.
        Color3,         // Must be a uncompressed color format.
        Color4,         // Must be a uncompressed color format.
        Color5,         // Must be a uncompressed color format.
        Color6,         // Must be a uncompressed color format.
        Color7,         // Must be a uncompressed color format.
        Depth,          // Must be a texture with a depth format.
        DepthStencil,   // Must be a texture with a depth/stencil format.
        NumAttachmentPoints
    };

    class RenderTarget
    {
    public:
        RenderTarget();
	    virtual ~RenderTarget() = default;

        static Ref<RenderTarget> Create();

        void AttachTexture(AttachmentPoint attachment, const Ref<Texture>& texture);

        Ref<Texture> GetTexture(AttachmentPoint attachment);

        void Clear(AttachmentPoint attachment, ClearFlags clearFlags = ClearFlags::All, const Vector4& color = Vector4::Zero, float depth = 1.0f, uint8_t stencil = 0) const;
        void Clear(ClearFlags clearFlags = ClearFlags::All, const Vector4& color = Vector4::Zero, float depth = 1.0f, uint8_t stencil = 0) const;

        void Resize(uint16_t width, uint16_t height);

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

	    [[nodiscard]] virtual bool IsValid() const = 0;

    protected:
        Vector<Ref<Texture>> m_Textures{};

        // The width in pixels of textures associated to this render target.
        uint16_t m_Width = 0;
        // The height in pixels of textures associated to this render target.
        uint16_t m_Height = 0;

        // Check to see if the render target is valid.
        bool m_CheckValidity = true;
    };
}
