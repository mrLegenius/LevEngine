#include "levpch.h"
#include "ParticleRenderingPass.h"

#include "ParticleAssets.h"
#include "Renderer/BlendState.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/DepthStencilState.h"
#include "Renderer/PipelineState.h"
#include "Renderer/RasterizerState.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderParams.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderType.h"
#include "Renderer/StructuredBuffer.h"
#include "Renderer/Texture.h"
#include "Renderer/Camera/SceneCamera.h"

namespace LevEngine
{
    ParticleRenderingPass::ParticleRenderingPass(
        const Ref<RenderTarget>& renderTarget,
        const Ref<StructuredBuffer>& particlesBuffer,
        const Ref<StructuredBuffer>& sortedBuffer)
            : m_ParticlesBuffer(particlesBuffer)
            , m_SortedBuffer(sortedBuffer)
            , m_PipelineState(CreateRef<PipelineState>())
            , m_CameraData(ConstantBuffer::Create(sizeof ParticleCameraData, 0))
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
        {
            const ParticleCameraData cameraData{ params.CameraViewMatrix, params.Camera->GetProjection(), params.CameraPosition };
            m_CameraData->SetData(&cameraData);
            m_CameraData->Bind(ShaderType::Vertex | ShaderType::Geometry);
        }
	
        ParticleShaders::Rendering()->Bind();

        m_ParticlesBuffer->Bind(0, ShaderType::Vertex, false);
        m_SortedBuffer->Bind(2, ShaderType::Vertex, false);

        m_PipelineState->Bind();

        //TODO: Restore textures
        ParticleTextures::Default()->Bind(1, ShaderType::Pixel);
        //for (uint32_t i = 0; i < textureSlotIndex; i++)
        //	textureSlots[i]->Bind(i+1, ShaderType::Pixel);

        const uint32_t particlesCount = m_SortedBuffer->GetCounterValue();
        RenderCommand::DrawPointList(particlesCount);

        //<--- Clean ---<<
        m_ParticlesBuffer->Unbind(1, ShaderType::Vertex, false);
        m_SortedBuffer->Unbind(2, ShaderType::Vertex, false);
        m_PipelineState->Unbind();
        ParticleTextures::Default()->Unbind(0, ShaderType::Pixel);
        ParticleShaders::Rendering()->Unbind();

        //TODO: Restore textures
        //for (uint32_t i = 0; i < textureSlotIndex; i++)
        //	textureSlots[i]->Unbind(i+1, ShaderType::Pixel);
    }

    void ParticleRenderingPass::SetViewport(const Viewport viewport)
    {
        m_PipelineState->GetRasterizerState().SetViewport(viewport);
    }
}
