#include "levpch.h"
#include "PlanetAtmospherePass.h"

#include "AtmosphereConstants.h"
#include "EnvironmentShaders.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderParams.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Pipeline/BlendState.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Pipeline/DepthStencilState.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Pipeline/RasterizerState.h"
#include "Renderer/Pipeline/Texture.h"
#include "Renderer/Shader/ShaderType.h"

namespace LevEngine
{
	namespace
	{
		//<--- CB_PLANET_ATMOSPHERE and T_FOG_DEPTH in Registers.hlsli ---<<
		constexpr uint32_t k_ConstantBufferSlot = 12;
		constexpr uint32_t k_DepthTextureSlot = 4;
	}

	PlanetAtmospherePass::PlanetAtmospherePass(const Ref<RenderTarget>& renderTarget,
	                                           const Ref<Texture>& depthTexture,
	                                           const Ref<AtmosphereConstants>& atmosphere)
		: m_DepthTexture(depthTexture), m_Atmosphere(atmosphere)
	{
		LEV_PROFILE_FUNCTION();

		m_Pipeline = CreateRef<PipelineState>();
		m_Pipeline->SetRenderTarget(renderTarget);
		m_Pipeline->GetRasterizerState().SetCullMode(CullMode::None);
		m_Pipeline->GetRasterizerState().SetDepthClipEnabled(false);
		m_Pipeline->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthTesting);
		m_Pipeline->GetDepthStencilState()->SetStencilMode(StencilMode{false});

		// Add the light the air scatters in, and keep the fraction of the scene it does not swallow.
		// That is source plus destination times one minus source alpha, which is ordinary alpha
		// blending with the source left unmultiplied -- the shader hands over radiance, not a colour to
		// mix towards.
		const BlendMode blend{
			true, false,
			BlendFactor::One, BlendFactor::OneMinusSrcAlpha, BlendOperation::Add,
			BlendFactor::One, BlendFactor::OneMinusSrcAlpha, BlendOperation::Add,
		};

		m_Pipeline->GetBlendState()->SetBlendMode(blend);

		m_ConstantBuffer = ConstantBuffer::Create(sizeof GPUPlanetAtmosphereData, k_ConstantBufferSlot);
	}

	String PlanetAtmospherePass::PassName() { return "Planet Atmosphere"; }

	bool PlanetAtmospherePass::Begin(entt::registry& registry, RenderParams& params)
	{
		LEV_PROFILE_FUNCTION();

		//<--- No atmosphere in the scene, so there is no air in front of anything ---<<
		if (!m_Atmosphere || !m_Atmosphere->IsActive()) return false;

		//<--- Reassigned every frame so an edited shader is picked up, as the sky pass does ---<<
		m_Pipeline->SetShader(ShaderType::Vertex, EnvironmentShaders::PlanetAtmosphere());
		m_Pipeline->SetShader(ShaderType::Pixel, EnvironmentShaders::PlanetAtmosphere());

		GPUPlanetAtmosphereData data;
		data.InverseViewProjection = params.CameraPerspectiveViewProjectionMatrix.Invert();
		data.CameraPosition = params.CameraPosition;
		data.ViewStepCount = static_cast<float>(RenderSettings::AtmosphereViewSteps);
		data.LightStepCount = static_cast<float>(RenderSettings::AtmosphereLightSteps);

		m_ConstantBuffer->SetData(&data);
		m_ConstantBuffer->Bind(ShaderType::Pixel);

		//<--- The scattering constants themselves: the same buffer the sky was drawn from ---<<
		m_Atmosphere->Bind();

		m_DepthTexture->Bind(k_DepthTextureSlot, ShaderType::Pixel);

		m_Pipeline->Bind();

		return RenderPass::Begin(registry, params);
	}

	void PlanetAtmospherePass::Process(entt::registry& registry, RenderParams& params)
	{
		LEV_PROFILE_FUNCTION();

		RenderCommand::DrawFullScreenQuad();
	}

	void PlanetAtmospherePass::End(entt::registry& registry, RenderParams& params)
	{
		m_Pipeline->Unbind();

		m_DepthTexture->Unbind(k_DepthTextureSlot, ShaderType::Pixel);
		m_ConstantBuffer->Unbind(ShaderType::Pixel);
	}

	void PlanetAtmospherePass::SetViewport(const Viewport viewport)
	{
		m_Pipeline->GetRasterizerState().SetViewport(viewport);
	}
}
