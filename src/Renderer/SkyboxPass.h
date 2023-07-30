#pragma once
#include "Renderer3D.h"
#include "RenderPass.h"
#include "entt/entt.hpp"

namespace LevEngine
{
class SkyboxPass final : public RenderPass
{
public:
	explicit SkyboxPass();

	void Process(entt::registry& registry, RenderParams& params) override;

private:

	PipelineState m_SkyboxPipeline;
	Ref<SkyboxMesh> m_SkyboxMesh;
	Ref<ConstantBuffer> m_CameraConstantBuffer;
};
}