#pragma once
#include "Renderer/RenderPass.h"

namespace LevEngine
{
    class StructuredBuffer;

    class ParticleSimulationPass final : public RenderPass
    {
    public:
        ParticleSimulationPass(const Ref<StructuredBuffer>& particlesBuffer, const Ref<StructuredBuffer>& deadBuffer, const Ref<StructuredBuffer>& sortedBuffer);
    protected:
        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;
        
    private:
        Ref<StructuredBuffer> m_ParticlesBuffer;
        Ref<StructuredBuffer> m_DeadBuffer;
        Ref<StructuredBuffer> m_SortedBuffer;
    };
}
