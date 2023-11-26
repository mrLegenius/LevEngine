#pragma once

#include "Renderer/RenderCommands.h"

namespace LevEngine
{
class D3D11RenderCommands final : public RenderCommands
{
public:
	~D3D11RenderCommands() override = default;

	void DrawIndexed(const Ref<IndexBuffer>& indexBuffer) override;
	void DrawFullScreenQuad() override;
	void DrawLineList(const Ref<IndexBuffer>& indexBuffer) override;
	void DrawLineStrip(uint32_t vertexCount) override;
	void DrawPointList(uint32_t count) override;
};
}
