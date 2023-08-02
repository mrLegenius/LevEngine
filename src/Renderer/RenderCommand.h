#pragma once
#include <cstdint>

#include "D3D11RendererAPI.h"
namespace LevEngine
{
class RenderCommand
{
public:
	static void SetViewport(int x, int y, uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewport(x, y, width, height);
	}

	static void DrawIndexed(
		const Ref<VertexBuffer>& vertexBuffer,
		const Ref<IndexBuffer>& indexBuffer)
	{
		s_RendererAPI->DrawIndexed(vertexBuffer, indexBuffer);
	}

	static void DrawFullScreenQuad()
	{
		s_RendererAPI->DrawFullScreenQuad();
	}

	static void SetLineWidth(float width)
	{
		s_RendererAPI->SetLineWidth(width);
	}

	static void DrawPointList(const uint32_t count)
	{
		s_RendererAPI->DrawPointList(count);
	}

private:
	static D3D11RendererAPI* s_RendererAPI;
};
}
