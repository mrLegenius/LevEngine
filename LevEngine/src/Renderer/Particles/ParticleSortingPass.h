#pragma once
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
    class BitonicSort;
    class StructuredBuffer;

    class ParticleSortingPass final : public RenderPass
    {
    public:
        explicit ParticleSortingPass(const Ref<StructuredBuffer>& sortedBuffer);
        ~ParticleSortingPass() override;
    protected:
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;
    private:
        Ref<StructuredBuffer> m_SortedBuffer;
        
        Ref<StructuredBuffer> m_TempBuffer{};
        Ref<BitonicSort> m_BitonicSort{};
    };
}
