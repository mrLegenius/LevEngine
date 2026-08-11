#pragma once

#include "Kernel/Core.h"
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
    class LEV_API ParticleSimulationPass final : public RenderPass
    {
    public:
        ParticleSimulationPass();
        ~ParticleSimulationPass() override;
    protected:
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;
    };
}
