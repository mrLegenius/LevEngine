#include "levpch.h"
#include "ParticleSimulationPass.h"

#include "ParticlesUtils.h"
#include "Renderer/DispatchCommand.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderType.h"
#include "Renderer/StructuredBuffer.h"

namespace LevEngine
{
    ParticleSimulationPass::ParticleSimulationPass(
    const Ref<StructuredBuffer>& particlesBuffer,
        const Ref<StructuredBuffer>& deadBuffer,
        const Ref<StructuredBuffer>& sortedBuffer)
            : m_ParticlesBuffer(particlesBuffer)
            , m_DeadBuffer(deadBuffer)
            , m_SortedBuffer(sortedBuffer) { }

    ParticleSimulationPass::~ParticleSimulationPass() = default;

    String ParticleSimulationPass::PassName() { return "Particle Simulation"; }

    bool ParticleSimulationPass::Begin(entt::registry& registry, RenderParams& params)
    {
        m_ParticlesBuffer->Bind(0, ShaderType::Compute, true, -1);
        m_DeadBuffer->Bind(1, ShaderType::Compute, true, -1);
        m_SortedBuffer->Bind(2, ShaderType::Compute, true, 0);

        return RenderPass::Begin(registry, params);
    }

    void ParticleSimulationPass::Process(entt::registry& registry, RenderParams& params)
    {
        //TODO: Bind depth and normal maps here to enable bounce again

        int groupSizeX = 0;
        int groupSizeY = 0;
        ParticlesUtils::GetGroupSize(RenderSettings::MaxParticles, groupSizeX, groupSizeY);

        ShaderAssets::ParticlesCompute()->Bind();
        DispatchCommand::Dispatch(groupSizeX, groupSizeY, 1);
        ShaderAssets::ParticlesCompute()->Unbind();
    }

    void ParticleSimulationPass::End(entt::registry& registry, RenderParams& params)
    {
        m_ParticlesBuffer->Unbind(0, ShaderType::Compute, true);
        m_DeadBuffer->Unbind(1, ShaderType::Compute, true);
        m_SortedBuffer->Unbind(2, ShaderType::Compute, true);
    }
}
