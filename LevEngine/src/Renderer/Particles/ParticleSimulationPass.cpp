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
#include "Scene/Components/Emitter/EmitterComponent.h"

namespace LevEngine
{
    ParticleSimulationPass::ParticleSimulationPass() { }

    ParticleSimulationPass::~ParticleSimulationPass() = default;

    String ParticleSimulationPass::PassName() { return "Particle Simulation"; }

    bool ParticleSimulationPass::Begin(entt::registry& registry, RenderParams& params)
    {
        return RenderPass::Begin(registry, params);
    }

    void ParticleSimulationPass::Process(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();
        //TODO: Bind depth and normal maps here to enable bounce again

        const auto group = registry.view<EmitterComponent>();
        for (const auto entity : group)
        {
            auto& emitter = group.get<EmitterComponent>(entity);

            if (!emitter.Buffers) continue;
            
            emitter.Buffers->GetParticlesBuffer()->Bind(0, ShaderType::Compute, true, -1);
            emitter.Buffers->GetDeadBuffer()->Bind(1, ShaderType::Compute, true, -1);
            emitter.Buffers->GetSorterBuffer()->Bind(2, ShaderType::Compute, true, 0);
        
            int groupSizeX = 0;
            int groupSizeY = 0;
            ParticlesUtils::GetGroupSize(emitter.Buffers->GetMaxParticlesCount(), groupSizeX, groupSizeY);

            ParticleShaders::Simulation()->Bind();
            DispatchCommand::Dispatch(groupSizeX, groupSizeY, 1);
            ParticleShaders::Simulation()->Unbind();

            emitter.Buffers->GetParticlesBuffer()->Unbind(0, ShaderType::Compute, true);
            emitter.Buffers->GetDeadBuffer()->Unbind(1, ShaderType::Compute, true);
            emitter.Buffers->GetSorterBuffer()->Unbind(2, ShaderType::Compute, true);
        }
    }

    void ParticleSimulationPass::End(entt::registry& registry, RenderParams& params)
    {
    
    }
}
