#include "pch.h"
#include "ParticlePass.h"

#include "Assets.h"
#include "Kernel/Time.h"
#include "Kernel/Application.h"
#include "RenderCommand.h"
#include "RenderSettings.h"
#include "Math/Random.h"
#include "Platform/D3D11/D3D11SamplerState.h"
#include "Platform/D3D11/D3D11Texture.h"

namespace LevEngine
{
extern ID3D11DeviceContext* context;

void BindTextureArray(Ref<D3D11Texture>* textures, const uint32_t count)
{
	auto** srv = new ID3D11ShaderResourceView *[count];
	for (int i = 0; i < count; ++i)
		srv[i] = textures[i]->GetShaderResourceView();

	auto ss = new ID3D11SamplerState* [count];
	for (int i = 0; i < count; ++i)
		ss[i] = std::dynamic_pointer_cast<D3D11SamplerState>(textures[i]->GetSamplerState())->GetSamplerState();

	context->PSSetShaderResources(0, count, srv);
	context->PSSetSamplers(0, count, ss);
}

ParticlePass::ParticlePass(const Ref<PipelineState>& pipelineState, const Ref<Texture>& depthTexture, const Ref<Texture>& normalTexture) : m_PipelineState(pipelineState), m_DepthTexture(depthTexture), m_NormalTexture(normalTexture)
{
	LEV_PROFILE_FUNCTION();

	m_CameraData = ConstantBuffer::Create(sizeof ParticleCameraData, 0);
	m_ComputeData = ConstantBuffer::Create(sizeof Handler, 1);
	m_EmitterData = ConstantBuffer::Create(sizeof Emitter, 2);
	m_RandomData = ConstantBuffer::Create(sizeof RandomGPUData, 3);

	auto particles = new GPUParticleData[RenderSettings::MaxParticles];
	auto indices = new uint32_t[RenderSettings::MaxParticles];

	for (int i = 0; i < RenderSettings::MaxParticles; ++i)
	{
		//particles[i].Age = -1;
		indices[i] = RenderSettings::MaxParticles - 1 - i;
	}

	m_ParticlesBuffer = StructuredBuffer::Create(particles, RenderSettings::MaxParticles, sizeof GPUParticleData, CPUAccess::None, true);
	m_DeadBuffer = StructuredBuffer::Create(indices, RenderSettings::MaxParticles, sizeof uint32_t, CPUAccess::None, true, StructuredBuffer::UAVType::Append);
	m_SortedBuffer = StructuredBuffer::Create(nullptr, RenderSettings::MaxParticles, sizeof Vector2, CPUAccess::None, true, StructuredBuffer::UAVType::Counter);
	m_TempBuffer = StructuredBuffer::Create(nullptr, RenderSettings::MaxParticles, sizeof Vector2, CPUAccess::None, true);

	m_DeadBuffer->Bind(1, Shader::Type::Compute, true, RenderSettings::MaxParticles);
	m_DeadBuffer->Unbind(1, Shader::Type::Compute, true);

	m_BitonicSort = CreateRef<BitonicSort>(RenderSettings::MaxParticles);

	delete[] particles;
	delete[] indices;

	srand(time(nullptr));
}

ParticlePass::Emitter ParticlePass::GetEmitterData(EmitterComponent emitter, Transform transform, uint32_t textureIndex)
{
	LEV_PROFILE_FUNCTION();

	RandomColor color{ emitter.Birth.StartColor, emitter.Birth.StartColorB, emitter.Birth.RandomStartColor };
	RandomVector3 velocity{  (Vector4)emitter.Birth.Velocity, emitter.Birth.VelocityB, emitter.Birth.RandomVelocity };
	RandomVector3 position{ (Vector4)transform.GetWorldPosition() + emitter.Birth.Position, transform.GetWorldPosition() + emitter.Birth.PositionB, emitter.Birth.RandomStartPosition };
	RandomFloat size{  emitter.Birth.StartSize, emitter.Birth.StartSizeB, emitter.Birth.RandomStartSize };
	RandomFloat lifeTime{  emitter.Birth.LifeTime, emitter.Birth.LifeTimeB, emitter.Birth.RandomStartLifeTime };

	auto emitterData = Emitter{
			Emitter::BirthParams{
				velocity,
				position,
				color,
				emitter.Birth.EndColor,
				size,
				emitter.Birth.EndSize,
				lifeTime,
				textureIndex,
				emitter.Birth.GravityScale,
			},
	};
	return emitterData;
}

bool ParticlePass::Begin(entt::registry& registry, RenderParams& params)
{
	LEV_PROFILE_FUNCTION();

	m_NormalTexture->Bind(8, Shader::Type::Pixel);
	m_DepthTexture->Bind(9, Shader::Type::Pixel);
	return RenderPass::Begin(registry, params);
}

void ParticlePass::Process(entt::registry& registry, RenderParams& params)
{
	LEV_PROFILE_FUNCTION();

	constexpr uint32_t MaxTextureSlots = 32;
	std::array<Ref<Texture>, MaxTextureSlots> textureSlots{};
	textureSlots[0] = TextureAssets::Particle();  //<--- default particle ---<<
	uint32_t textureSlotIndex = 1;

	const float deltaTime = Time::GetDeltaTime().GetSeconds();

	const auto group = registry.view<Transform, EmitterComponent, IDComponent>();
	m_ParticlesBuffer->Bind(0, Shader::Type::Compute, true, -1);

	int groupSizeX = 0;
	int groupSizeY = 0;

	GetGroupSize(RenderSettings::MaxParticles, groupSizeX, groupSizeY);

	const Handler handler{ groupSizeY, RenderSettings::MaxParticles, deltaTime};
	m_ComputeData->SetData(&handler);
	m_ComputeData->Bind(Shader::Type::Compute);

	const ParticleCameraData cameraData{ params.CameraViewMatrix, params.Camera.GetProjection(), params.CameraPosition };
	m_CameraData->SetData(&cameraData);
	m_CameraData->Bind(Shader::Type::Compute);

	m_DeadBuffer->Bind(1, Shader::Type::Compute, true, -1);
	
	for (const auto entity : group)
	{
		auto [transform, emitter, id] = group.get<Transform, EmitterComponent, IDComponent>(entity);

		int textureIndex = -1.0f;

		for (uint32_t i = 0; i < textureSlotIndex; i++)
		{
			if (!emitter.Texture)
			{
				textureIndex = 0;
				break;
			}

			if (textureSlots[i]->GetPath() == emitter.Texture->GetPath())
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == -1.0f)
		{
			if (textureSlotIndex >= MaxTextureSlots)
			{
				textureIndex = 0;
			}
			else
			{
				textureIndex = textureSlotIndex;
				textureSlots[textureSlotIndex] = emitter.Texture;
				textureSlotIndex++;
			}
		}

		auto emitterData = GetEmitterData(emitter, transform, textureIndex);
		m_EmitterData->SetData(&emitterData);
		m_EmitterData->Bind(Shader::Type::Compute);

		RandomGPUData randomData{ Random::Int(0, std::numeric_limits<int>::max())};
		m_RandomData->SetData(&randomData);
		m_RandomData->Bind(Shader::Type::Compute);

		//<--- Emit ---<<
		
		ShaderAssets::ParticlesEmitter()->Bind();
		const uint32_t deadParticlesCount = m_DeadBuffer->GetCounterValue();
		auto particlesToEmit = std::min(deadParticlesCount, static_cast<uint32_t>(emitter.Rate));
		if (particlesToEmit > 0)
				context->Dispatch(particlesToEmit, 1, 1);
	}

	//<--- Simulate ---<<

	//TODO: Bind depth and normal maps here to enable bounce again
	m_DeadBuffer->Bind(1, Shader::Type::Compute, true, -1);
	m_SortedBuffer->Bind(2, Shader::Type::Compute, true, 0);

	ShaderAssets::ParticlesCompute()->Bind();
	context->Dispatch(groupSizeX, groupSizeY, 1);

	ShaderAssets::ParticlesCompute()->Unbind();

	m_ParticlesBuffer->Unbind(0, Shader::Type::Compute, true);
	m_DeadBuffer->Unbind(1, Shader::Type::Compute, true);
	m_SortedBuffer->Unbind(2, Shader::Type::Compute, true);

	//<--- Sort ---<<
	m_BitonicSort->Sort(m_SortedBuffer, m_TempBuffer);

	//<--- Render ---<<
	{
		const ParticleCameraData cameraData{ params.CameraViewMatrix, params.Camera.GetProjection(), params.CameraPosition };
		m_CameraData->SetData(&cameraData);
		m_CameraData->Bind(Shader::Type::Vertex | Shader::Type::Geometry);
	}
	
	ShaderAssets::Particles()->Bind();

	m_ParticlesBuffer->Bind(0, Shader::Type::Vertex, false);
	m_SortedBuffer->Bind(2, Shader::Type::Vertex, false);

	m_PipelineState->Bind();

	//TextureAssets::Particle()->Bind(1);
	for (uint32_t i = 0; i < textureSlotIndex; i++)
		textureSlots[i]->Bind(i+1, Shader::Type::Pixel);

	const uint32_t particlesCount = m_SortedBuffer->GetCounterValue();
	RenderCommand::DrawPointList(particlesCount);

	//<--- Clean ---<<
	m_ParticlesBuffer->Unbind(0, Shader::Type::Vertex, false);
	m_SortedBuffer->Unbind(2, Shader::Type::Vertex, false);
	m_PipelineState->Unbind();
	ShaderAssets::Particles()->Unbind();
	TextureAssets::Particle()->Unbind(1, Shader::Type::Pixel);
	for (uint32_t i = 0; i < textureSlotIndex; i++)
		textureSlots[i]->Unbind(i+1, Shader::Type::Pixel);
}

void ParticlePass::End(entt::registry& registry, RenderParams& params)
{
	LEV_PROFILE_FUNCTION();

	m_NormalTexture->Unbind(8, Shader::Type::Pixel);
	m_DepthTexture->Unbind(9, Shader::Type::Pixel);
}

void ParticlePass::GetGroupSize(const int totalCount, int& groupSizeX, int& groupSizeY) const
{
	LEV_PROFILE_FUNCTION();

	const int numGroups = (totalCount % c_MaxThreadCount != 0) ? ((totalCount / c_MaxThreadCount) + 1) : (totalCount / c_MaxThreadCount);
	const double secondRoot = std::ceil(std::pow(static_cast<double>(numGroups), 0.5));
	groupSizeX = static_cast<int>(secondRoot);
	groupSizeY = static_cast<int>(secondRoot);
}
}
