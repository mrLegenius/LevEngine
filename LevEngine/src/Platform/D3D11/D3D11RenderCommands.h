#pragma once
#include <d3d11_2.h>

#include "Renderer/RenderCommands.h"

namespace LevEngine
{
class D3D11RenderCommands final : public RenderCommands
{
public:
	D3D11RenderCommands(ID3D11Device2* device);
	~D3D11RenderCommands() override = default;

	void DrawIndexed(const Ref<IndexBuffer>& indexBuffer) override;
	void DrawFullScreenQuad() override;
	void DrawLineList(const Ref<IndexBuffer>& indexBuffer) override;
	void DrawLineStrip(uint32_t vertexCount) override;
	void DrawPointList(uint32_t count) override;

private:
	ID3D11DeviceContext2* m_DeviceContext;
};
}
