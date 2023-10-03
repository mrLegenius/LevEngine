#pragma once
#include "ConstantBuffer.h"
#include "PipelineState.h"
#include "RenderPass.h"

namespace LevEngine
{
class OpaquePass final : public RenderPass
{
public:
	explicit OpaquePass(const Ref<PipelineState>& pipelineState);

	bool Begin(entt::registry& registry, RenderParams& params) override;
	void Process(entt::registry& registry, RenderParams& params) override;
	void End(entt::registry& registry, RenderParams& params) override;

private:
	struct alignas(16) CameraData
	{
		Matrix View;
		Matrix ViewProjection;
		Vector3 Position;
	};

	Ref<PipelineState> m_PipelineState;
};
}
