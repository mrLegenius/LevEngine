#pragma once
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
    class StructuredBuffer;
    class BitonicSort;
    class ParticleBuffers;

    class ParticleSortingPass final : public RenderPass
    {
    public:
        explicit ParticleSortingPass(const Ref<ParticleBuffers>& buffers);
        ~ParticleSortingPass() override;
    protected:
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;
    private:
        Ref<ParticleBuffers> m_Buffers;
        
        Ref<StructuredBuffer> m_TempBuffer{};
        Ref<BitonicSort> m_BitonicSort{};
    };
}
