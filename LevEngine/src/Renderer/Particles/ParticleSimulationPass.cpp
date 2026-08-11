#include "levpch.h"
#include "ParticleSimulationPass.h"

#include "ParticleAssets.h"
#include "ParticleBuffers.h"
#include "ParticlesUtils.h"
#include "Renderer/Dispatch/DispatchCommand.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Shader/ShaderType.h"
#include "Renderer/Pipeline/StructuredBuffer.h"
#include "Scene/Components/Emitter/EmitterComponent.h"

namespace LevEngine
{
    ParticleSimulationPass::ParticleSimulationPass() = default;

    ParticleSimulationPass::~ParticleSimulationPass() = default;

    String ParticleSimulationPass::PassName() { return "Particle Simulation"; }

    void ParticleSimulationPass::Process(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();
        //TODO: Bind depth and normal maps here to enable bounce again

        const auto group = registry.view<EmitterComponent>();
        for (const auto entity : group)
        {
            auto& emitter = group.get<EmitterComponent>(entity);

            const auto& buffers = emitter.GetBuffers();
            if (!buffers) continue;

            buffers->GetParticlesBuffer()->Bind(0, ShaderType::Compute, true, -1);
            buffers->GetDeadBuffer()->Bind(1, ShaderType::Compute, true, -1);
            buffers->GetSorterBuffer()->Bind(2, ShaderType::Compute, true, 0);

            int groupSizeX = 0;
            int groupSizeY = 0;
            ParticlesUtils::GetGroupSize(buffers->GetMaxParticlesCount(), groupSizeX, groupSizeY);

            ParticleShaders::Simulation()->Bind();
            DispatchCommand::Dispatch(groupSizeX, groupSizeY, 1);
            ParticleShaders::Simulation()->Unbind();

            buffers->GetParticlesBuffer()->Unbind(0, ShaderType::Compute, true);
            buffers->GetDeadBuffer()->Unbind(1, ShaderType::Compute, true);
            buffers->GetSorterBuffer()->Unbind(2, ShaderType::Compute, true);
        }
    }

}
