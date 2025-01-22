#include "levpch.h"
#include "ParticleSortingPass.h"

#include "BitonicSort.h"
#include "ParticleBuffers.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Pipeline/StructuredBuffer.h"

namespace LevEngine
{
    ParticleSortingPass::ParticleSortingPass(const Ref<ParticleBuffers>& buffers)
        : m_Buffers(buffers)
        , m_TempBuffer(StructuredBuffer::Create(nullptr, RenderSettings::MaxParticles, sizeof Vector2, CPUAccess::None, true))
        , m_BitonicSort(CreateRef<BitonicSort>(RenderSettings::MaxParticles))
    { }

    ParticleSortingPass::~ParticleSortingPass() = default;

    String ParticleSortingPass::PassName() { return "Particle Sorting"; }

    void ParticleSortingPass::Process(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();
        
        m_BitonicSort->Sort(m_Buffers->GetSorterBuffer(), m_TempBuffer);
    }
}
