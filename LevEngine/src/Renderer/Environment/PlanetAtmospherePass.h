#pragma once

#include "Kernel/Core.h"
#include "Math/Math.h"
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
	class AtmosphereConstants;
	class ConstantBuffer;
	class PipelineState;
	class RenderTarget;
	class Texture;

	// Mirrors PlanetAtmosphereParams in PlanetAtmosphere.hlsl.
	struct LEV_API alignas(16) GPUPlanetAtmosphereData
	{
		Matrix InverseViewProjection;

		Vector3 CameraPosition{};
		float ViewStepCount = 8.0f;

		float LightStepCount = 4.0f;
		Vector3 Padding{};
	};

	// The air in front of the scene, as opposed to the air behind it.
	//
	// The sky pass covers the pixels with nothing drawn in them. This covers the rest: it marches the
	// same atmosphere over the stretch of view ray between the camera and the surface it can see, which
	// is what tints distant ground and what draws the bright limb over a planet's edge rather than
	// leaving a halo detached outside it.
	//
	// Runs where the fog does -- over the lit HDR target, before tone mapping, so the air goes through
	// the same exposure as everything else, and after everything that writes colour or depth.
	class LEV_API PlanetAtmospherePass final : public RenderPass
	{
	public:
		// Takes the atmosphere the environment pass already collected rather than collecting again:
		// there is one atmosphere in a scene and its constant buffer is filled once a frame.
		PlanetAtmospherePass(const Ref<RenderTarget>& renderTarget, const Ref<Texture>& depthTexture,
		                     const Ref<AtmosphereConstants>& atmosphere);

	protected:
		String PassName() override;
		bool Begin(entt::registry& registry, RenderParams& params) override;
		void Process(entt::registry& registry, RenderParams& params) override;
		void End(entt::registry& registry, RenderParams& params) override;

	public:
		void SetViewport(Viewport viewport) override;

	private:
		Ref<PipelineState> m_Pipeline;
		Ref<Texture> m_DepthTexture;
		Ref<AtmosphereConstants> m_Atmosphere;
		Ref<ConstantBuffer> m_ConstantBuffer;
	};
}
