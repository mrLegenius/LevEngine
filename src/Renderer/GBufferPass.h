#pragma once
#include "ConstantBuffer.h"
#include "PipelineState.h"
#include "RenderPass.h"

namespace LevEngine
{
class GBufferPass final : public RenderPass
{
public:
	explicit GBufferPass(const Ref<PipelineState>& pipelineState)
		: m_PipelineState(pipelineState)
	{
		m_CameraConstantBuffer = ConstantBuffer::Create(sizeof CameraData, 0);

	}

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
	Ref<ConstantBuffer> m_CameraConstantBuffer;
};
}
