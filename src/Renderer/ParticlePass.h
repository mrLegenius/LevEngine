#pragma once

#include "BitonicSort.h"
#include "PipelineState.h"
#include "RenderPass.h"
#include "entt/entt.hpp""
#include "Scene/Components/Components.h"

class ParticlePass : public RenderPass
{
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
		uint32_t TextureIndex;
		float GravityScale;
	};

	struct ParticleCameraData
	{
		Matrix View;
		Matrix Projection;
		alignas(16) Vector3 Position;
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
			float GravityScale;
		};

		BirthParams Birth;
	};

public:
	ParticlePass(entt::registry& registry);
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
	Ref<D3D11StructuredBuffer> m_TempBuffer{};

	Ref<BitonicSort> m_BitonicSort{};

	void GetGroupSize(int totalCount, int& groupSizeX, int& groupSizeY) const;

	entt::registry& m_Registry;
};
