#include "levpch.h"
#include "ParticlePass.h"

#include "ParticleBuffers.h"
#include "ParticleEmissionPass.h"
#include "ParticleRenderingPass.h"
#include "ParticleSimulationPass.h"
#include "ParticleSortingPass.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Shader/Shader.h"
#include "Math/Random.h"
#include "Renderer/Pipeline/Texture.h"
#include "ParticlesTextureArray.h"
#include "Kernel/Time/Time.h"
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

        m_EmissionPass = CreateScope<ParticleEmissionPass>();
        m_SimulationPass = CreateScope<ParticleSimulationPass>();
        m_SortingPass = CreateScope<ParticleSortingPass>();
        m_RenderingPass = CreateScope<ParticleRenderingPass>(renderTarget);
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

        static uint32_t updateFrame = -1;

        if (updateFrame != Time::GetFrameNumber())
        {
            updateFrame = Time::GetFrameNumber();
            
            m_EmissionPass->Execute(registry, params);
            m_SimulationPass->Execute(registry, params);
        }

        //m_SortingPass->Execute(registry, params);
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
