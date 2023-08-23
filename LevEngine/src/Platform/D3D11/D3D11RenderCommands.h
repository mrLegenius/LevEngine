#pragma once

#include "Renderer/RenderCommands.h"

#include "Renderer/IndexBuffer.h"
#include "Renderer/VertexBuffer.h"

namespace LevEngine
{
class D3D11RenderCommands final : public RenderCommands
{
public:
	~D3D11RenderCommands() override = default;

	void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer) override;
	void DrawFullScreenQuad() override;
	void DrawPointList(uint32_t count) override;
};
}
