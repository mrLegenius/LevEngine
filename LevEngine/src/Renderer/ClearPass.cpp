#include "levpch.h"
#include "ClearPass.h"

#include "RenderTarget.h"
#include "Texture.h"

namespace LevEngine
{
    ClearPass::ClearPass(const Ref<RenderTarget>& renderTarget,
                         const String& label,
                         const ClearFlags clearFlags,
                         const Vector4& color,
                         const float depth,
                         const uint8_t stencil)
        : m_RenderTarget(renderTarget)
          , m_ClearFlags(clearFlags)
          , m_ClearColor(color)
          , m_ClearDepth(depth)
          , m_ClearStencil(stencil)
          , m_Label(label)
    {
    }

    ClearPass::ClearPass(const Ref<Texture>& texture,
                         const String& label,
                         const ClearFlags clearFlags,
                         const Vector4& color, const float depth,
                         const uint8_t stencil)
        : m_Texture(texture)
          , m_ClearFlags(clearFlags)
          , m_ClearColor(color)
          , m_ClearDepth(depth)
          , m_ClearStencil(stencil)
          , m_Label(label)
    {
    }

    String ClearPass::PassName() { return m_Label; }

    void ClearPass::Process(entt::registry&, RenderParams&)
    {
        if (m_RenderTarget)
            m_RenderTarget->Clear(m_ClearFlags, m_ClearColor, m_ClearDepth, m_ClearStencil);

        if (m_Texture)
            m_Texture->Clear(m_ClearFlags, m_ClearColor, m_ClearDepth, m_ClearStencil);
    }
}
