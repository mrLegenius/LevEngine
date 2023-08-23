#pragma once
#include "RenderPass.h"
#include "PipelineState.h"
#include "3D/SkyboxMesh.h"

namespace LevEngine
{
class SkyboxPass final : public RenderPass
{
public:
	explicit SkyboxPass(const Ref<PipelineState>& pipeline);
	void Process(entt::registry& registry, RenderParams& params) override;

private:

	Ref<PipelineState> m_SkyboxPipeline;
	Ref<SkyboxMesh> m_SkyboxMesh;
	Ref<ConstantBuffer> m_CameraConstantBuffer;
};
}