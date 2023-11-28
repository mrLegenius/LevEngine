#include "levpch.h"
#include "ParticlePass.h"

#include "Assets/EngineAssets.h"
#include "BitonicSort.h"
#include "ParticleEmitterPass.h"
#include "ParticlesUtils.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/PipelineState.h"
#include "Kernel/Time/Time.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderParams.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Shader.h"
#include "Renderer/StructuredBuffer.h"
#include "Assets/TextureAsset.h"
#include "Renderer/Camera/SceneCamera.h"
#include "Math/Random.h"
#include "Platform/D3D11/D3D11SamplerState.h"
#include "Platform/D3D11/D3D11Texture.h"
#include "Scene/Components/Components.h"

namespace LevEngine
{
extern ID3D11DeviceContext* context;

void BindTextureArray(const Ref<D3D11Texture>* textures, const uint32_t count)
{
	auto** srv = new ID3D11ShaderResourceView *[count];
	for (uint32_t i = 0; i < count; ++i)
		srv[i] = textures[i]->GetShaderResourceView();

	const auto ss = new ID3D11SamplerState* [count];
	for (uint32_t i = 0; i < count; ++i)
		ss[i] = CastRef<D3D11SamplerState>(textures[i]->GetSamplerState())->GetSamplerState();

	context->PSSetShaderResources(0, count, srv);
	context->PSSetSamplers(0, count, ss);
}

ParticlePass::ParticlePass(const Ref<PipelineState>& pipelineState, const Ref<Texture>& depthTexture, const Ref<Texture>& normalTexture) : m_PipelineState(pipelineState), m_DepthTexture(depthTexture), m_NormalTexture(normalTexture)
{
	LEV_PROFILE_FUNCTION();

	m_CameraData = ConstantBuffer::Create(sizeof ParticleCameraData, 0);
	
	const auto particles = new GPUParticleData[RenderSettings::MaxParticles];
	const auto indices = new uint32_t[RenderSettings::MaxParticles];

	for (uint32_t i = 0; i < RenderSettings::MaxParticles; ++i)
	{
		//particles[i].Age = -1;
		indices[i] = RenderSettings::MaxParticles - 1 - i;
	}

	m_ParticlesBuffer = StructuredBuffer::Create(particles, RenderSettings::MaxParticles, sizeof GPUParticleData, CPUAccess::None, true);
	m_DeadBuffer = StructuredBuffer::Create(indices, RenderSettings::MaxParticles, sizeof uint32_t, CPUAccess::None, true, StructuredBuffer::UAVType::Append);
	m_SortedBuffer = StructuredBuffer::Create(nullptr, RenderSettings::MaxParticles, sizeof Vector2, CPUAccess::None, true, StructuredBuffer::UAVType::Counter);
	m_TempBuffer = StructuredBuffer::Create(nullptr, RenderSettings::MaxParticles, sizeof Vector2, CPUAccess::None, true);

	m_DeadBuffer->Bind(1, ShaderType::Compute, true, RenderSettings::MaxParticles);
	m_DeadBuffer->Unbind(1, ShaderType::Compute, true);

	m_BitonicSort = CreateRef<BitonicSort>(RenderSettings::MaxParticles);

	m_ParticleEmitterPass = CreateScope<ParticleEmitterPass>(m_ParticlesBuffer, m_DeadBuffer);
	
	delete[] particles;
	delete[] indices;
}


String ParticlePass::PassName() { return "Particles"; }

bool ParticlePass::Begin(entt::registry& registry, RenderParams& params)
{
	LEV_PROFILE_FUNCTION();

	m_NormalTexture->Bind(8, ShaderType::Pixel);
	m_DepthTexture->Bind(9, ShaderType::Pixel);
	return RenderPass::Begin(registry, params);
}

void ParticlePass::Process(entt::registry& registry, RenderParams& params)
{
	LEV_PROFILE_FUNCTION();

	m_ParticleEmitterPass->Execute(registry, params);

	//<--- Simulate ---<<

	//TODO: Bind depth and normal maps here to enable bounce again
	m_DeadBuffer->Bind(1, ShaderType::Compute, true, -1);
	m_SortedBuffer->Bind(2, ShaderType::Compute, true, 0);

	int groupSizeX = 0;
	int groupSizeY = 0;
	ParticlesUtils::GetGroupSize(RenderSettings::MaxParticles, groupSizeX, groupSizeY);
	
	ShaderAssets::ParticlesCompute()->Bind();
	context->Dispatch(groupSizeX, groupSizeY, 1);

	ShaderAssets::ParticlesCompute()->Unbind();

	m_ParticlesBuffer->Unbind(0, ShaderType::Compute, true);
	m_DeadBuffer->Unbind(1, ShaderType::Compute, true);
	m_SortedBuffer->Unbind(2, ShaderType::Compute, true);

	//<--- Sort ---<<
	m_BitonicSort->Sort(m_SortedBuffer, m_TempBuffer);

	//<--- Render ---<<
	{
		const ParticleCameraData cameraData{ params.CameraViewMatrix, params.Camera->GetProjection(), params.CameraPosition };
		m_CameraData->SetData(&cameraData);
		m_CameraData->Bind(ShaderType::Vertex | ShaderType::Geometry);
	}
	
	ShaderAssets::Particles()->Bind();

	m_ParticlesBuffer->Bind(0, ShaderType::Vertex, false);
	m_SortedBuffer->Bind(2, ShaderType::Vertex, false);

	m_PipelineState->Bind();

	//TODO: Restore textures
	TextureAssets::Particle()->Bind(1, ShaderType::Pixel);
	//for (uint32_t i = 0; i < textureSlotIndex; i++)
	//	textureSlots[i]->Bind(i+1, ShaderType::Pixel);

	const uint32_t particlesCount = m_SortedBuffer->GetCounterValue();
	RenderCommand::DrawPointList(particlesCount);

	//<--- Clean ---<<
	m_ParticlesBuffer->Unbind(0, ShaderType::Vertex, false);
	m_SortedBuffer->Unbind(2, ShaderType::Vertex, false);
	m_PipelineState->Unbind();
	ShaderAssets::Particles()->Unbind();
	TextureAssets::Particle()->Unbind(1, ShaderType::Pixel);

	//TODO: Restore textures
	//for (uint32_t i = 0; i < textureSlotIndex; i++)
	//	textureSlots[i]->Unbind(i+1, ShaderType::Pixel);
}

void ParticlePass::End(entt::registry& registry, RenderParams& params)
{
	LEV_PROFILE_FUNCTION();

	m_NormalTexture->Unbind(8, ShaderType::Pixel);
	m_DepthTexture->Unbind(9, ShaderType::Pixel);
}
}
