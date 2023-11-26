#pragma once
#include "RenderPass.h"

namespace LevEngine
{
	class EnvironmentPass;
	class ConstantBuffer;
	class Texture;
	class Mesh;
	class PipelineState;

	class SkyboxPass final : public RenderPass
	{
	public:
		explicit SkyboxPass(const Ref<PipelineState>& pipeline, const Ref<EnvironmentPass>& environmentPass);
		bool Begin(entt::registry& registry, RenderParams& params) override;
		void Process(entt::registry& registry, RenderParams& params) override;

	private:
        Ref<PipelineState> m_SkyboxPipeline;
        Ref<EnvironmentPass> m_EnvironmentPass;
        
        Ref<Mesh> m_SkyboxMesh;
        Ref<ConstantBuffer> m_CameraConstantBuffer;
    };
}
