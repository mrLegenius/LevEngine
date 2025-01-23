#include "levpch.h"
#include "ParticleBuffers.h"

#include "GPUParticleData.h"
#include "Renderer/Pipeline/CPUAccess.h"
#include "Renderer/Pipeline/StructuredBuffer.h"
#include "Renderer/Pipeline/UAVType.h"
#include "Renderer/Shader/ShaderType.h"

namespace LevEngine
{
    ParticleBuffers::ParticleBuffers(uint32_t maxParticles) : m_MaxParticles(maxParticles)
    {
        const auto particles = new GPUParticleData[maxParticles];
        const auto indices = new uint32_t[maxParticles];

        for (uint32_t i = 0; i < maxParticles; ++i)
        {
            particles[i].Age = -1;
            indices[i] = i;
        }

        m_ParticlesBuffer = StructuredBuffer::Create(particles, maxParticles, sizeof GPUParticleData,
                                                     CPUAccess::None, true);
        m_DeadBuffer = StructuredBuffer::Create(indices, maxParticles, sizeof uint32_t, CPUAccess::None,
                                                true, UAVType::Append);
        m_SortedBuffer = StructuredBuffer::Create(nullptr, maxParticles, sizeof Vector2,
                                                  CPUAccess::None, true, UAVType::Counter);

        m_DeadBuffer->Bind(1, ShaderType::Compute, true, maxParticles);
        m_DeadBuffer->Unbind(1, ShaderType::Compute, true);

        delete[] particles;
        delete[] indices;
    }
}
