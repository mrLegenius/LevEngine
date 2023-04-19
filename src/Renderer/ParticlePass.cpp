#include "ParticlePass.h"

#include "Assets.h"
#include "Random.h"
#include "Kernel/Time.h"
#include "Kernel/Application.h"
#include "RenderCommand.h"

ParticlePass::ParticlePass(entt::registry& registry): m_Registry(registry)
{
	const auto mainRenderTarget = Application::Get().GetWindow().GetContext()->GetRenderTarget();
	m_PipelineState.GetBlendState().SetBlendMode(BlendMode::AlphaBlending);
	m_PipelineState.GetDepthStencilState().SetDepthMode(DepthMode{ true, DepthWrite::Disable });
	m_PipelineState.GetRasterizerState().SetCullMode(CullMode::None);
	m_PipelineState.SetRenderTarget(mainRenderTarget);

	m_CameraData = CreateRef<D3D11ConstantBuffer>(sizeof ParticleCameraData, 0);
	m_ComputeData = CreateRef<D3D11ConstantBuffer>(sizeof Handler, 0);
	m_EmitterData = CreateRef<D3D11ConstantBuffer>(sizeof Emitter, 1);
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
	const float deltaTime = LevEngine::Time::GetDeltaTime().GetSeconds();

	const auto group = m_Registry.view<Transform, EmitterComponent, IDComponent>();
	for (const auto entity : group)
	{
		auto [transform, emitter, id] = group.get<Transform, EmitterComponent, IDComponent>(entity);

		const Ref<ParticleSystem> particleSystem = GetParticleSystem(emitter, id);

		auto emitterData = GetEmitterData(emitter, transform);
		m_EmitterData->SetData(&emitterData);

		int groupSizeX = 0;
		int groupSizeY = 0;

		GetGroupSize(emitter.MaxParticles, groupSizeX, groupSizeY);

		const Handler handler{ groupSizeY, emitter.MaxParticles, deltaTime };
		m_ComputeData->SetData(&handler);

		//<--- Emit ---<<
		particleSystem->PrepareToEmit(emitter.Rate);


		ShaderAssets::ParticlesEmitter()->Bind();
		context->Dispatch(emitter.Rate, 1, 1);

		//<--- Simulate ---<<
		particleSystem->PrepareToSimulate();

		ShaderAssets::ParticlesCompute()->Bind();
		context->Dispatch(groupSizeX, groupSizeY, 1);

		ShaderAssets::ParticlesCompute()->Unbind();

		//<--- Render ---<<
		const ParticleCameraData cameraData{ params.CameraViewMatrix, params.Camera.GetProjection() };
		m_CameraData->SetData(&cameraData);

		m_PipelineState.Bind();
		particleSystem->PrepareToRender();
		ShaderAssets::Particles()->Bind();

		if (emitter.Texture)
			emitter.Texture->Bind(1);
		else
			TextureAssets::Particle()->Bind(1);

		const uint32_t particlesCount = particleSystem->GetAliveParticlesCount();
		RenderCommand::DrawPointList(particlesCount);

		//<--- Clean ---<<
		particleSystem->End();
		m_PipelineState.Unbind();
		ShaderAssets::Particles()->Unbind();
		if (emitter.Texture)
			emitter.Texture->Unbind(1);
		else
			TextureAssets::Particle()->Unbind(1);
	}
}

void ParticlePass::GetGroupSize(const int totalCount, int& groupSizeX, int& groupSizeY) const
{
	const int numGroups = (totalCount % c_MaxThreadCount != 0) ? ((totalCount / c_MaxThreadCount) + 1) : (totalCount / c_MaxThreadCount);
	const double secondRoot = std::ceil(std::pow(static_cast<double>(numGroups), 0.5));
	groupSizeX = static_cast<int>(secondRoot);
	groupSizeY = static_cast<int>(secondRoot);
}
