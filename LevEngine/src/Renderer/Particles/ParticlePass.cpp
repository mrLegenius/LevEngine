#include "levpch.h"
#include "ParticlePass.h"

#include "ParticleEmissionPass.h"
#include "ParticleRenderingPass.h"
#include "ParticleSimulationPass.h"
#include "ParticleSortingPass.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Pipeline/StructuredBuffer.h"
#include "Math/Random.h"
#include "Renderer/Pipeline/Texture.h"
#include "ParticlesTextureArray.h"
#include "Renderer/RenderParams.h"
#include "Renderer/Camera/SceneCamera.h"
#include "Renderer/Pipeline/ConstantBuffer.h"

namespace LevEngine
{
    ParticlePass::ParticlePass(const Ref<RenderTarget>& renderTarget, const Ref<Texture>& depthTexture,
                               const Ref<Texture>& normalTexture)
        : m_CameraData(ConstantBuffer::Create(sizeof ParticleCameraData, 0)), m_DepthTexture(depthTexture)
        , m_NormalTexture(normalTexture)
    {
        LEV_PROFILE_FUNCTION();

        const auto particles = new GPUParticleData[RenderSettings::MaxParticles];
        const auto indices = new uint32_t[RenderSettings::MaxParticles];

        for (uint32_t i = 0; i < RenderSettings::MaxParticles; ++i)
        {
            particles[i].Age = -1;
            indices[i] = RenderSettings::MaxParticles - 1 - i;
        }

        m_ParticlesBuffer = StructuredBuffer::Create(particles, RenderSettings::MaxParticles, sizeof GPUParticleData,
                                                     CPUAccess::None, true);
        m_DeadBuffer = StructuredBuffer::Create(indices, RenderSettings::MaxParticles, sizeof uint32_t, CPUAccess::None,
                                                true, UAVType::Append);
        m_SortedBuffer = StructuredBuffer::Create(nullptr, RenderSettings::MaxParticles, sizeof Vector2,
                                                  CPUAccess::None, true, UAVType::Counter);

        m_DeadBuffer->Bind(1, ShaderType::Compute, true, RenderSettings::MaxParticles);
        m_DeadBuffer->Unbind(1, ShaderType::Compute, true);

        m_ParticlesTextures = CreateRef<ParticlesTextureArray>();
        m_EmissionPass = CreateScope<ParticleEmissionPass>(m_ParticlesBuffer, m_DeadBuffer, m_ParticlesTextures);
        m_SimulationPass = CreateScope<ParticleSimulationPass>(m_ParticlesBuffer, m_DeadBuffer, m_SortedBuffer);
        m_SortingPass = CreateScope<ParticleSortingPass>(m_SortedBuffer);
        m_RenderingPass = CreateScope<ParticleRenderingPass>(renderTarget, m_ParticlesBuffer, m_SortedBuffer, m_ParticlesTextures);

        delete[] particles;
        delete[] indices;
    }

    String ParticlePass::PassName() { return "Particles"; }

    bool ParticlePass::Begin(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        m_NormalTexture->Bind(8, ShaderType::Pixel);
        m_DepthTexture->Bind(9, ShaderType::Pixel);

        const ParticleCameraData cameraData{ params.CameraViewMatrix, params.Camera->GetProjection(), params.CameraPosition };
        m_CameraData->SetData(&cameraData);
        m_CameraData->Bind(ShaderType::Vertex | ShaderType::Geometry | ShaderType::Compute);

        return RenderPass::Begin(registry, params);
    }

    void ParticlePass::Process(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        m_EmissionPass->Execute(registry, params);
        m_SimulationPass->Execute(registry, params);
        m_SortingPass->Execute(registry, params);
        m_RenderingPass->Execute(registry, params);
    }

    void ParticlePass::End(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        m_CameraData->Unbind(ShaderType::Vertex | ShaderType::Geometry | ShaderType::Compute);
        
        m_NormalTexture->Unbind(8, ShaderType::Pixel);
        m_DepthTexture->Unbind(9, ShaderType::Pixel);
    }

    void ParticlePass::SetViewport(const Viewport viewport)
    {
        m_RenderingPass->SetViewport(viewport);
    }
}
