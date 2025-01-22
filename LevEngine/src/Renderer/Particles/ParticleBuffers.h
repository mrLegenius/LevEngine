#pragma once
namespace LevEngine
{
    class StructuredBuffer;

    class ParticleBuffers
    {
    public:
        ParticleBuffers(uint32_t maxParticles);
        const Ref<StructuredBuffer>& GetParticlesBuffer() const { return m_ParticlesBuffer; }
        const Ref<StructuredBuffer>& GetDeadBuffer() const { return m_DeadBuffer; }
        const Ref<StructuredBuffer>& GetSorterBuffer() const { return m_SortedBuffer; }
    private:
        Ref<StructuredBuffer> m_ParticlesBuffer{};
        Ref<StructuredBuffer> m_DeadBuffer{};
        Ref<StructuredBuffer> m_SortedBuffer{};
    };
}
