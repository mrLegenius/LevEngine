#pragma once
#include "D3D11StructuredBuffer.h"
#include "RenderParams.h"
#include "Kernel/PointerUtils.h"

struct GPUParticleData
{
	Vector3 Position;
	Vector3 Velocity;

	Color StartColor;
	Color EndColor;
	Color Color;

	float StartSize;
	float EndSize;
	float Size;

	float LifeTime;
	float Age;
};

class ParticleSystem
{
public:
	explicit ParticleSystem(uint32_t maxParticles)
	{
		auto particles = new GPUParticleData[maxParticles];
		auto indices = new uint32_t[maxParticles];

		for (int i = 0; i < maxParticles; ++i)
		{
			particles[i].LifeTime = 0.0001;
			indices[i] = maxParticles - 1 - i;
		}

		m_ParticlesBuffer = CreateRef<D3D11StructuredBuffer>(particles, maxParticles, sizeof GPUParticleData, CPUAccess::None, true);
		m_DeadBuffer = CreateRef<D3D11StructuredBuffer>(indices, maxParticles, sizeof uint32_t, CPUAccess::None, true, D3D11StructuredBuffer::UAVType::Append);
		m_SortedBuffer = CreateRef<D3D11StructuredBuffer>(nullptr, maxParticles, sizeof Vector2, CPUAccess::None, true, D3D11StructuredBuffer::UAVType::Append);

		delete[] particles;
		delete[] indices;
	}

	void PrepareToEmit(float emitRate) const
	{
		m_ParticlesBuffer->Bind(0, ShaderType::Compute, true, -1);
		m_DeadBuffer->Bind(1, ShaderType::Compute, true, emitRate - 1);
		m_SortedBuffer->Bind(2, ShaderType::Compute, true, 0);
	}

	void PrepareToSimulate() const
	{
		m_DeadBuffer->Bind(1, ShaderType::Compute, true, -1);
	}

	void PrepareToRender() const
	{
		m_ParticlesBuffer->Unbind(0, ShaderType::Compute, true);
		m_DeadBuffer->Unbind(1, ShaderType::Compute, true);
		m_SortedBuffer->Unbind(2, ShaderType::Compute, true);

		m_ParticlesBuffer->Bind(0, ShaderType::Vertex, false);
		m_SortedBuffer->Bind(2, ShaderType::Vertex, false);
	}

	uint32_t GetAliveParticlesCount() const
	{
		return m_SortedBuffer->GetCounterValue();
	}

	void End() const
	{
		m_ParticlesBuffer->Unbind(0, ShaderType::Vertex, false);
		m_SortedBuffer->Unbind(2, ShaderType::Vertex, false);
	}

private:
	Ref<D3D11StructuredBuffer> m_ParticlesBuffer;
	Ref<D3D11StructuredBuffer> m_DeadBuffer;
	Ref<D3D11StructuredBuffer> m_SortedBuffer;
};
