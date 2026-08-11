#pragma once

#include "Kernel/Core.h"
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
    class StructuredBuffer;
    class BitonicSort;
    class ParticleBuffers;

    class LEV_API ParticleSortingPass final : public RenderPass
    {
    public:
        ParticleSortingPass();
        ~ParticleSortingPass() override;
    protected:
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;
    private:
        //<--- Sorting resources have to match the size of the sorted buffer, so they are cached per size ---<<
        struct SortingResources
        {
            Ref<StructuredBuffer> TempBuffer;
            Ref<BitonicSort> Sort;
        };

        const SortingResources& GetSortingResources(uint32_t maxParticles);

        UnorderedMap<uint32_t, SortingResources> m_SortingResources;
    };
}
