#include "levpch.h"
#include "ParticleSortingPass.h"

#include "BitonicSort.h"
#include "GPUParticleData.h"
#include "ParticleBuffers.h"
#include "Renderer/Pipeline/StructuredBuffer.h"
#include "Scene/Components/Emitter/EmitterComponent.h"

namespace LevEngine
{
    ParticleSortingPass::ParticleSortingPass() = default;

    ParticleSortingPass::~ParticleSortingPass() = default;

    String ParticleSortingPass::PassName() { return "Particle Sorting"; }

    const ParticleSortingPass::SortingResources& ParticleSortingPass::GetSortingResources(const uint32_t maxParticles)
    {
        const auto it = m_SortingResources.find(maxParticles);
        if (it != m_SortingResources.end()) return it->second;

        SortingResources resources;
        resources.TempBuffer = StructuredBuffer::Create(nullptr, maxParticles, sizeof SortedParticleData, CPUAccess::None, true);
        resources.Sort = CreateRef<BitonicSort>(static_cast<int>(maxParticles));

        return m_SortingResources.emplace(maxParticles, Move(resources)).first->second;
    }

    void ParticleSortingPass::Process(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        const auto group = registry.view<EmitterComponent>();
        for (const auto entity : group)
        {
            auto& emitter = group.get<EmitterComponent>(entity);

            const auto& buffers = emitter.GetBuffers();
            if (!buffers) continue;

            const auto& resources = GetSortingResources(buffers->GetMaxParticlesCount());

            resources.Sort->Sort(buffers->GetSorterBuffer(), resources.TempBuffer);
        }
    }
}
