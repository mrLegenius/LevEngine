#pragma once

#include "Kernel/Core.h"
namespace LevEngine
{
    class StructuredBuffer;

    class LEV_API ParticleBuffers
    {
    public:
        explicit ParticleBuffers(uint32_t maxParticles);
        [[nodiscard]] uint32_t GetMaxParticlesCount() const { return m_MaxParticles; }
        const Ref<StructuredBuffer>& GetParticlesBuffer() const { return m_ParticlesBuffer; }
        const Ref<StructuredBuffer>& GetDeadBuffer() const { return m_DeadBuffer; }
        const Ref<StructuredBuffer>& GetSorterBuffer() const { return m_SortedBuffer; }
    private:
        uint32_t m_MaxParticles;
        
        Ref<StructuredBuffer> m_ParticlesBuffer{};
        Ref<StructuredBuffer> m_DeadBuffer{};
        Ref<StructuredBuffer> m_SortedBuffer{};
    };
}
