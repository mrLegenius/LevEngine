#include "levpch.h"
#include "ParticleSimulationPass.h"

#include "ParticleAssets.h"
#include "ParticleBuffers.h"
#include "ParticlesUtils.h"
#include "Renderer/Dispatch/DispatchCommand.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Shader/ShaderType.h"
#include "Renderer/Pipeline/StructuredBuffer.h"

namespace LevEngine
{
    ParticleSimulationPass::ParticleSimulationPass(const Ref<ParticleBuffers>& buffers)
        : m_Buffers(buffers){ }

    ParticleSimulationPass::~ParticleSimulationPass() = default;

    String ParticleSimulationPass::PassName() { return "Particle Simulation"; }

    bool ParticleSimulationPass::Begin(entt::registry& registry, RenderParams& params)
    {
        m_Buffers->GetParticlesBuffer()->Bind(0, ShaderType::Compute, true, -1);
        m_Buffers->GetDeadBuffer()->Bind(1, ShaderType::Compute, true, -1);
        m_Buffers->GetSorterBuffer()->Bind(2, ShaderType::Compute, true, 0);

        return RenderPass::Begin(registry, params);
    }

    void ParticleSimulationPass::Process(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();
        //TODO: Bind depth and normal maps here to enable bounce again

        int groupSizeX = 0;
        int groupSizeY = 0;
        ParticlesUtils::GetGroupSize(RenderSettings::MaxParticles, groupSizeX, groupSizeY);

        ParticleShaders::Simulation()->Bind();
        DispatchCommand::Dispatch(groupSizeX, groupSizeY, 1);
        ParticleShaders::Simulation()->Unbind();
    }

    void ParticleSimulationPass::End(entt::registry& registry, RenderParams& params)
    {
        m_Buffers->GetParticlesBuffer()->Unbind(0, ShaderType::Compute, true);
        m_Buffers->GetDeadBuffer()->Unbind(1, ShaderType::Compute, true);
        m_Buffers->GetSorterBuffer()->Unbind(2, ShaderType::Compute, true);
    }
}
