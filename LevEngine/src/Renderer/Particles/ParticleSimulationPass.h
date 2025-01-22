#pragma once
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
    class ParticleBuffers;
    class StructuredBuffer;

    class ParticleSimulationPass final : public RenderPass
    {
    public:
        ParticleSimulationPass(const Ref<ParticleBuffers>& buffers);
        ~ParticleSimulationPass() override;
    protected:
        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;
        
    private:
        Ref<ParticleBuffers> m_Buffers;
    };
}
