#include "levpch.h"
#include "ClearPass.h"
namespace LevEngine
{
ClearPass::ClearPass(const Ref<RenderTarget>& renderTarget, const ClearFlags clearFlags, const Vector4& color, const float depth, const uint8_t stencil)
    : m_RenderTarget(renderTarget)
    , m_ClearFlags(clearFlags)
    , m_ClearColor(color)
    , m_ClearDepth(depth)
    , m_ClearStencil(stencil)
{}

ClearPass::ClearPass(const Ref<Texture>& texture, const ClearFlags clearFlags, const Vector4& color, const float depth, const uint8_t stencil)
    : m_Texture(texture)
    , m_ClearFlags(clearFlags)
    , m_ClearColor(color)
    , m_ClearDepth(depth)
    , m_ClearStencil(stencil)
{}

void ClearPass::Process(entt::registry&, RenderParams&)
{
    if (m_RenderTarget)
	    m_RenderTarget->Clear(m_ClearFlags, m_ClearColor, m_ClearDepth, m_ClearStencil);

    if (m_Texture)
	    m_Texture->Clear(m_ClearFlags, m_ClearColor, m_ClearDepth, m_ClearStencil);
}
}