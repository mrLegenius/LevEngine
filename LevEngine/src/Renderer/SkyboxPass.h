#pragma once
#include "RenderPass.h"

namespace LevEngine
{
	class ConstantBuffer;
	class Mesh;
	class PipelineState;

	class SkyboxPass final : public RenderPass
{
public:
	explicit SkyboxPass(const Ref<PipelineState>& pipeline);
	void Process(entt::registry& registry, RenderParams& params) override;
	void CreateCubeMapSubPass(const Ref<Texture>& skyboxTexture);

private:

	Ref<PipelineState> m_SkyboxPipeline;
	Ref<Mesh> m_SkyboxMesh;
	Ref<ConstantBuffer> m_CameraConstantBuffer;
	Ref<Texture> m_CubemapTexture;
};
}
