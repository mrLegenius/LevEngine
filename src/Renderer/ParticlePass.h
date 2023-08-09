#pragma once

#include "BitonicSort.h"
#include "PipelineState.h"
#include "RenderPass.h"
#include "entt/entt.hpp"
#include "Scene/Components/Components.h"

namespace LevEngine
{
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


	struct RandomFloat
	{
		float From;
		float To;
		int Randomize = false;

		float pad;
	};

	struct RandomVector3
	{
		alignas(16) Vector4 From;
		Vector3 To;
		int Randomize = false;
	};

	struct RandomColor
	{
		alignas(16) Color From;
		alignas(16) Color To;
		int Randomize = false;

		float pad[3];
	};

	struct Emitter
	{
		struct BirthParams
		{
			alignas(16) RandomVector3 Velocity;
			alignas(16) RandomVector3 Position;

			alignas(16) RandomColor StartColor;
			alignas(16) Color EndColor;

			alignas(16) RandomFloat StartSize;
			float EndSize;
			alignas(16) RandomFloat LifeTime;

			//<--- 16 byte ---<<
			uint32_t TextureIndex;
			float GravityScale;
		};

		BirthParams Birth;
	};

	struct RandomGPUData
	{
		alignas(16) int RandomSeed;
	};

public:
	ParticlePass(const Ref<PipelineState>& pipelineState, const Ref<Texture>& depthTexture, const Ref<Texture>& normalTexture);
	static Emitter GetEmitterData(EmitterComponent emitter, Transform transform, uint32_t textureIndex);

	bool Begin(entt::registry& registry, RenderParams& params) override;
	void Process(entt::registry& registry, RenderParams& params) override;
	void End(entt::registry& registry, RenderParams& params) override;

private:

	static constexpr uint32_t c_MaxThreadCount = 1024;

	Ref<StructuredBuffer> m_ParticlesBuffer;
	Ref<StructuredBuffer> m_DeadBuffer;
	Ref<StructuredBuffer> m_SortedBuffer;

	Ref<PipelineState> m_PipelineState{};
	Ref<ConstantBuffer> m_CameraData{};
	Ref<ConstantBuffer> m_ComputeData{};
	Ref<ConstantBuffer> m_EmitterData{};
	Ref<ConstantBuffer> m_RandomData{};
	Ref<StructuredBuffer> m_TempBuffer{};

	Ref<BitonicSort> m_BitonicSort{};

	Ref<Texture> m_DepthTexture;
	Ref<Texture> m_NormalTexture;

	void GetGroupSize(int totalCount, int& groupSizeX, int& groupSizeY) const;
};
}