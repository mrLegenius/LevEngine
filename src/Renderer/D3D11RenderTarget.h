#pragma once
#include <cstdint>
#include <SimpleMath.h>
#include <vector>

#include "ClearFlags.h"
#include "Texture.h"
#include "Math/Math.h"

#include "Platform/D3D11/D3D11Texture.h"

using namespace DirectX::SimpleMath;
namespace LevEngine
{
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

class D3D11RenderTarget
{
public:
    D3D11RenderTarget()
    {
        m_Textures.resize(static_cast<size_t>(AttachmentPoint::NumAttachmentPoints));
    }
    void AttachTexture(AttachmentPoint attachment, const Ref<Texture>& texture)
    {
	    const Ref<D3D11Texture> d3d11Texture = std::dynamic_pointer_cast<D3D11Texture>(texture);
        m_Textures[static_cast<uint8_t>(attachment)] = d3d11Texture;

        // Next time the render target is "bound", check that it is valid.
        m_CheckValidity = true;
    }

    std::shared_ptr<Texture> GetTexture(AttachmentPoint attachment) { return m_Textures[static_cast<uint8_t>(attachment)]; }

    void Clear(AttachmentPoint attachment, const ClearFlags clearFlags = ClearFlags::All, const Vector4& color = Vector4::Zero, const float depth = 1.0f, const uint8_t stencil = 0) const
    {
	    if (const auto& texture = m_Textures[static_cast<uint8_t>(attachment)])
	        texture->Clear(clearFlags, color, depth, stencil);
    }

    void Clear(const ClearFlags clearFlags = ClearFlags::All, const Vector4& color = Vector4::Zero, const float depth = 1.0f, const uint8_t stencil = 0) const
    {
        for (uint8_t i = 0; i < static_cast<uint8_t>(AttachmentPoint::NumAttachmentPoints); ++i)
        {
            Clear(static_cast<AttachmentPoint>(i), clearFlags, color, depth, stencil);
        }
    }

    void Resize(const uint16_t width, const uint16_t height)
    {
        if (m_Width != width || m_Height != height)
        {
            m_Width = Math::Max<uint16_t>(width, 1);
            m_Height = Math::Max<uint16_t>(height, 1);
            // Resize the attached textures
            for (auto texture : m_Textures)
            {
                if (texture)
	                texture->Resize(m_Width, m_Height);
            }
        }
    }

    void Bind();
    void Unbind();

    bool IsValid() const;
private:

    typedef std::vector<Ref<D3D11Texture>> TextureList;
    TextureList m_Textures{};

    // The width in pixels of textures associated to this render target.
    uint16_t m_Width = 0;
    // The height in pixels of textures associated to this render target.
    uint16_t m_Height = 0;

    // Check to see if the render target is valid.
    bool m_CheckValidity = true;
};
}