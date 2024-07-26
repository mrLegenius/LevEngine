#include "levpch.h"
#include "QuadRenderPass.h"

#include "Renderer/RenderCommand.h"
#include "Renderer/Shader/ShaderType.h"
#include "Renderer/Pipeline/Texture.h"

namespace LevEngine
{
    String QuadRenderPass::PassName() { return "Quad"; }

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
