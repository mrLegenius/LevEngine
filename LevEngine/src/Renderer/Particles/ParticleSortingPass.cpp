#include "levpch.h"
#include "ParticleSortingPass.h"

#include "BitonicSort.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Pipeline/StructuredBuffer.h"

namespace LevEngine
{
    ParticleSortingPass::ParticleSortingPass(const Ref<StructuredBuffer>& sortedBuffer)
        : m_SortedBuffer(sortedBuffer)
        , m_TempBuffer(StructuredBuffer::Create(nullptr, RenderSettings::MaxParticles, sizeof Vector2, CPUAccess::None, true))
        , m_BitonicSort(CreateRef<BitonicSort>(RenderSettings::MaxParticles))
    { }

    ParticleSortingPass::~ParticleSortingPass() = default;

    String ParticleSortingPass::PassName() { return "Particle Sorting"; }

    void ParticleSortingPass::Process(entt::registry& registry, RenderParams& params)
    {
        m_BitonicSort->Sort(m_SortedBuffer, m_TempBuffer);
    }
}
