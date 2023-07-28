#pragma once
#include "Renderer3D.h"
#include "RenderPass.h"
#include "entt/entt.hpp"

namespace LevEngine
{
class SkyboxPass final : public RenderPass
{
public:
	explicit SkyboxPass(entt::registry& registry);

	void Process(RenderParams& params) override;

private:

	entt::registry& m_Registry;
	PipelineState m_SkyboxPipeline;
};
}