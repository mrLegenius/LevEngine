#pragma once

#include "PipelineState.h"
#include "ParticleSystem.h"
#include "RenderPass.h"
#include "entt/entt.hpp""
#include "Scene/Components/Components.h"

extern ID3D11DeviceContext* context;

class ParticlePass : public RenderPass
{
	struct ParticleCameraData
	{
		Matrix View;
		Matrix Projection;
	};

	struct alignas(16) Handler
	{
		int GroupDim;
		uint32_t MaxParticles;
		float DeltaTime;
	};

	struct Emitter
	{
		struct BirthParams
		{
			alignas(16) Vector3 Velocity = Vector3::Zero;
			alignas(16) Vector3 Position = Vector3::Zero;

			alignas(16) Color StartColor;
			alignas(16) Color EndColor;

			//<--- 16 byte ---<<
			float StartSize;
			float EndSize;
			float LifeTime;
			uint32_t TextureIndex;
			//<--- 16 byte ---<<
		};

		BirthParams Birth;
	};

public:
	ParticlePass(entt::registry& registry);
	Ref<ParticleSystem> GetParticleSystem(EmitterComponent emitter, IDComponent id);
	Emitter GetEmitterData(EmitterComponent emitter, Transform transform) const;

	void Process(RenderParams& params) override;

private:

	static constexpr uint32_t c_MaxThreadCount = 1024;

	Ref<D3D11StructuredBuffer> m_ParticlesBuffer;
	Ref<D3D11StructuredBuffer> m_DeadBuffer;
	Ref<D3D11StructuredBuffer> m_SortedBuffer;

	PipelineState m_PipelineState;
	Ref<D3D11ConstantBuffer> m_CameraData{};
	Ref<D3D11ConstantBuffer> m_ComputeData{};
	Ref<D3D11ConstantBuffer> m_EmitterData{};

	std::unordered_map<LevEngine::UUID, Ref<ParticleSystem>> m_ParticleSystems{};

	void GetGroupSize(int totalCount, int& groupSizeX, int& groupSizeY) const;

	entt::registry& m_Registry;
};
