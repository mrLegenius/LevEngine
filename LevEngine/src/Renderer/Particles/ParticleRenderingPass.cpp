#include "levpch.h"
#include "ParticleRenderingPass.h"

#include "ParticleAssets.h"
#include "ParticleBuffers.h"
#include "Assets/TextureAsset.h"
#include "Renderer/Pipeline/BlendState.h"
#include "Renderer/Pipeline/DepthStencilState.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Pipeline/RasterizerState.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Shader/ShaderType.h"
#include "Renderer/Pipeline/StructuredBuffer.h"
#include "Renderer/Pipeline/Texture.h"
#include "Scene/Components/Emitter/EmitterComponent.h"

namespace LevEngine
{
    ParticleRenderingPass::ParticleRenderingPass(const Ref<RenderTarget>& renderTarget)
            : m_PipelineState(CreateRef<PipelineState>())
        {
        m_PipelineState->GetBlendState()->SetBlendMode(BlendMode::AlphaBlending);
        m_PipelineState->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthWrites);
        m_PipelineState->GetRasterizerState().SetCullMode(CullMode::None);
        m_PipelineState->SetRenderTarget(renderTarget);
    }

    ParticleRenderingPass::~ParticleRenderingPass() = default;

    String ParticleRenderingPass::PassName() { return "Particle Rendering"; }

    void ParticleRenderingPass::Process(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();
        
        ParticleShaders::Rendering()->Bind();
        m_PipelineState->Bind();
        
        const auto group = registry.view<EmitterComponent>();
        for (const auto entity : group)
        {
            auto& emitter = group.get<EmitterComponent>(entity);

            const auto& buffers = emitter.GetBuffers();
            if (!buffers) continue;

            buffers->GetParticlesBuffer()->Bind(0, ShaderType::Vertex, false);
            buffers->GetSorterBuffer()->Bind(2, ShaderType::Vertex, false);

            const Ref<Texture>& texture = emitter.Texture ? emitter.Texture->GetTexture() : ParticleTextures::Default();
            texture->Bind(1, ShaderType::Pixel);

            RenderCommand::DrawPointList(buffers->GetMaxParticlesCount());

            //<--- Clean, otherwise the buffers stay bound as SRVs and collide with the compute passes ---<<
            buffers->GetParticlesBuffer()->Unbind(0, ShaderType::Vertex, false);
            buffers->GetSorterBuffer()->Unbind(2, ShaderType::Vertex, false);

            texture->Unbind(1, ShaderType::Pixel);
        }

        ParticleShaders::Rendering()->Unbind();
        m_PipelineState->Unbind();
    }

    void ParticleRenderingPass::SetViewport(const Viewport viewport)
    {
        m_PipelineState->GetRasterizerState().SetViewport(viewport);
    }
}
