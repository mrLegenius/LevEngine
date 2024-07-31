#include "levpch.h"
#include "ParticleRenderingPass.h"

#include "ParticleAssets.h"
#include "ParticlesTextureArray.h"
#include "Renderer/Pipeline/BlendState.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Pipeline/DepthStencilState.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Pipeline/RasterizerState.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderParams.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Shader/ShaderType.h"
#include "Renderer/Pipeline/StructuredBuffer.h"
#include "Renderer/Pipeline/Texture.h"
#include "Renderer/Camera/SceneCamera.h"

namespace LevEngine
{
    ParticleRenderingPass::ParticleRenderingPass(
        const Ref<RenderTarget>& renderTarget,
        const Ref<StructuredBuffer>& particlesBuffer,
        const Ref<StructuredBuffer>& sortedBuffer,
        const Ref<ParticlesTextureArray>& particlesTextures)
            : m_ParticlesBuffer(particlesBuffer)
            , m_SortedBuffer(sortedBuffer)
            , m_PipelineState(CreateRef<PipelineState>())
            , m_ParticlesTextures(particlesTextures)
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
        ParticleShaders::Rendering()->Bind();

        m_ParticlesBuffer->Bind(0, ShaderType::Vertex, false);
        m_SortedBuffer->Bind(2, ShaderType::Vertex, false);

        m_PipelineState->Bind();
        
        for (uint32_t i = 0; i < m_ParticlesTextures->TextureSlotIndex; i++)
        	m_ParticlesTextures->TextureSlots[i]->Bind(i+1, ShaderType::Pixel);

        const uint32_t particlesCount = m_SortedBuffer->GetCounterValue();
        RenderCommand::DrawPointList(particlesCount);

        //<--- Clean ---<<
        m_ParticlesBuffer->Unbind(1, ShaderType::Vertex, false);
        m_SortedBuffer->Unbind(2, ShaderType::Vertex, false);
        m_PipelineState->Unbind();
        ParticleShaders::Rendering()->Unbind();

        for (uint32_t i = 0; i < m_ParticlesTextures->TextureSlotIndex; i++)
            m_ParticlesTextures->TextureSlots[i]->Unbind(i+1, ShaderType::Pixel);
    }

    void ParticleRenderingPass::SetViewport(const Viewport viewport)
    {
        m_PipelineState->GetRasterizerState().SetViewport(viewport);
    }
}
