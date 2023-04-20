#include "ParticlePass.h"

#include "Assets.h"
#include "Random.h"
#include "Kernel/Time.h"
#include "Kernel/Application.h"
#include "RenderCommand.h"
#include "RenderSettings.h"

void BindTextureArray(Ref<D3D11Texture>* textures, uint32_t count)
{
	auto** srv = new ID3D11ShaderResourceView *[count];
	for (int i = 0; i < count; ++i)
		srv[i] = textures[i]->GetShaderResourceView();

	auto ss = new ID3D11SamplerState * [count];
	for (int i = 0; i < count; ++i)
		ss[i] = textures[i]->GetSamplerState();

	context->PSSetShaderResources(0, count, srv);
	context->PSSetSamplers(0, count, ss);
}

ParticlePass::ParticlePass(entt::registry& registry) : m_Registry(registry)
{
	const auto mainRenderTarget = Application::Get().GetWindow().GetContext()->GetRenderTarget();
	m_PipelineState.GetBlendState().SetBlendMode(BlendMode::AlphaBlending);
	m_PipelineState.GetDepthStencilState().SetDepthMode(DepthMode{ true, DepthWrite::Disable });
	m_PipelineState.GetRasterizerState().SetCullMode(CullMode::None);
	m_PipelineState.SetRenderTarget(mainRenderTarget);

	m_CameraData = CreateRef<D3D11ConstantBuffer>(sizeof ParticleCameraData, 0);
	m_ComputeData = CreateRef<D3D11ConstantBuffer>(sizeof Handler, 0);
	m_EmitterData = CreateRef<D3D11ConstantBuffer>(sizeof Emitter, 1);

	auto particles = new GPUParticleData[RenderSettings::MaxParticles];
	auto indices = new uint32_t[RenderSettings::MaxParticles];

	for (int i = 0; i < RenderSettings::MaxParticles; ++i)
	{
		//particles[i].Age = -1;
		indices[i] = i + 1; // RenderSettings::MaxParticles - i;
	}

	m_ParticlesBuffer = CreateRef<D3D11StructuredBuffer>(particles, RenderSettings::MaxParticles, sizeof GPUParticleData, CPUAccess::None, true);
	m_DeadBuffer = CreateRef<D3D11StructuredBuffer>(indices, RenderSettings::MaxParticles, sizeof uint32_t, CPUAccess::None, true, D3D11StructuredBuffer::UAVType::Append);
	m_SortedBuffer = CreateRef<D3D11StructuredBuffer>(nullptr, RenderSettings::MaxParticles, sizeof Vector2, CPUAccess::None, true, D3D11StructuredBuffer::UAVType::Append);

	m_DeadBuffer->Bind(1, ShaderType::Compute, true, RenderSettings::MaxParticles);
	m_DeadBuffer->Unbind(1, ShaderType::Compute, true);

	delete[] particles;
	delete[] indices;
}

Ref<ParticleSystem> ParticlePass::GetParticleSystem(EmitterComponent emitter, IDComponent id)
{
	Ref<ParticleSystem> particleSystem;
	if (auto iterator = m_ParticleSystems.find(id.ID); iterator == m_ParticleSystems.end())
	{
		particleSystem = CreateRef<ParticleSystem>(emitter.MaxParticles);
		m_ParticleSystems.emplace(id.ID, particleSystem);
	}
	else
	{
		particleSystem = iterator->second;
	}

	return particleSystem;
}

ParticlePass::Emitter ParticlePass::GetEmitterData(EmitterComponent emitter, Transform transform) const
{
	Color color = emitter.Birth.RandomStartColor
		? Random::Color(emitter.Birth.StartColor, emitter.Birth.StartColorB)
		: emitter.Birth.StartColor;

	Vector3 position = emitter.Birth.RandomStartPosition
		? Random::Vec3(emitter.Birth.Position, emitter.Birth.PositionB)
		: emitter.Birth.Position;

	float size = emitter.Birth.RandomStartSize
		? Random::Range(emitter.Birth.StartSize, emitter.Birth.StartSizeB)
		: emitter.Birth.StartSize;

	float lifeTime = emitter.Birth.RandomStartLifeTime
		? Random::Range(emitter.Birth.LifeTime, emitter.Birth.LifeTimeB)
		: emitter.Birth.LifeTime;

	auto emitterData = Emitter{
			Emitter::BirthParams{
				emitter.Birth.Velocity,
				transform.GetWorldPosition() + position,
				color,
				emitter.Birth.EndColor,
				size,
				emitter.Birth.EndSize,
				lifeTime,
			},
	};
	return emitterData;
}

void ParticlePass::Process(RenderParams& params)
{
	constexpr uint32_t MaxTextureSlots = 32;
	std::array<Ref<D3D11Texture>, MaxTextureSlots> textureSlots{};
	textureSlots[0] = TextureAssets::Particle();  //<--- default particle ---<<
	uint32_t textureSlotIndex = 1;

	const float deltaTime = LevEngine::Time::GetDeltaTime().GetSeconds();

	const auto group = m_Registry.view<Transform, EmitterComponent, IDComponent>();
	m_ParticlesBuffer->Bind(0, ShaderType::Compute, true, -1);

	int groupSizeX = 0;
	int groupSizeY = 0;

	GetGroupSize(RenderSettings::MaxParticles, groupSizeX, groupSizeY);

	const Handler handler{ groupSizeY, RenderSettings::MaxParticles, deltaTime };
	m_ComputeData->SetData(&handler);
	m_DeadBuffer->Bind(1, ShaderType::Compute, true, -1);
	
	for (const auto entity : group)
	{
		auto [transform, emitter, id] = group.get<Transform, EmitterComponent, IDComponent>(entity);

		const Ref<ParticleSystem> particleSystem = GetParticleSystem(emitter, id);

		auto emitterData = GetEmitterData(emitter, transform);
		m_EmitterData->SetData(&emitterData);

		//<--- Emit ---<<
		
		ShaderAssets::ParticlesEmitter()->Bind();
		context->Dispatch(emitter.Rate, 1, 1);

		/*float textureIndex = -1.0f;

		for (uint32_t i = 0; i < textureSlotIndex; i++)
		{
			if (textureSlots[i]->GetRenderTargetView() == emitter.Texture->GetRenderTargetView())
			{
				textureIndex = static_cast<float>(i);
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
				textureIndex = static_cast<float>(textureSlotIndex);
				textureSlots[textureSlotIndex] = emitter.Texture;
				textureSlotIndex++;
			}
		}*/
	}

	//<--- Simulate ---<<
	m_DeadBuffer->Bind(1, ShaderType::Compute, true, -1);
	m_SortedBuffer->Bind(2, ShaderType::Compute, true, 0);

	ShaderAssets::ParticlesCompute()->Bind();
	context->Dispatch(groupSizeX, groupSizeY, 1);

	ShaderAssets::ParticlesCompute()->Unbind();

	m_ParticlesBuffer->Unbind(0, ShaderType::Compute, true);
	m_DeadBuffer->Unbind(1, ShaderType::Compute, true);
	m_SortedBuffer->Unbind(2, ShaderType::Compute, true);
	//<--- Render ---<<

	m_ParticlesBuffer->Bind(0, ShaderType::Vertex, false);
	m_SortedBuffer->Bind(2, ShaderType::Vertex, false);

	const ParticleCameraData cameraData{ params.CameraViewMatrix, params.Camera.GetProjection() };
	m_CameraData->SetData(&cameraData);

	m_PipelineState.Bind();
	ShaderAssets::Particles()->Bind();

	TextureAssets::Particle()->Bind(1);
	/*for (uint32_t i = 0; i < textureSlotIndex; i++)
		textureSlots[i]->Bind(i);*/

	const uint32_t particlesCount = m_SortedBuffer->GetCounterValue();
	RenderCommand::DrawPointList(particlesCount);

	//<--- Clean ---<<
	m_ParticlesBuffer->Unbind(0, ShaderType::Vertex, false);
	m_SortedBuffer->Unbind(2, ShaderType::Vertex, false);
	m_PipelineState.Unbind();
	ShaderAssets::Particles()->Unbind();
	TextureAssets::Particle()->Unbind(1);
	/*for (uint32_t i = 0; i < textureSlotIndex; i++)
		textureSlots[i]->Unbind(i);*/
}

void ParticlePass::GetGroupSize(const int totalCount, int& groupSizeX, int& groupSizeY) const
{
	const int numGroups = (totalCount % c_MaxThreadCount != 0) ? ((totalCount / c_MaxThreadCount) + 1) : (totalCount / c_MaxThreadCount);
	const double secondRoot = std::ceil(std::pow(static_cast<double>(numGroups), 0.5));
	groupSizeX = static_cast<int>(secondRoot);
	groupSizeY = static_cast<int>(secondRoot);
}
