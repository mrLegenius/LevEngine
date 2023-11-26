#include "levpch.h"
#include "QuadRenderPass.h"

#include "RenderCommand.h"
#include "Texture.h"

namespace LevEngine
{
bool QuadRenderPass::Begin(entt::registry& registry, RenderParams& params)
{
    m_Pipeline->Bind();
    m_ColorTexture->Bind(0, ShaderType::Pixel);
    return RenderPass::Begin(registry, params);
}

void QuadRenderPass::Process(entt::registry& registry, RenderParams& params)
{
    RenderCommand::DrawFullScreenQuad();
}

void QuadRenderPass::End(entt::registry& registry, RenderParams& params)
{
    m_ColorTexture->Unbind(0, ShaderType::Pixel);
    m_Pipeline->Unbind();
}
}
