#pragma once
#include <cstdint>

#include "D3D11RendererAPI.h"
namespace LevEngine
{
class RenderCommand
{
public:
	static void Init()
	{
		s_RendererAPI->Init();
	}

	static void SetViewport(int x, int y, uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewport(x, y, width, height);
	}

	static void SetClearColor(float color[4])
	{
		s_RendererAPI->SetClearColor(color);
	}

	static void Clear()
	{
		s_RendererAPI->Clear();
	}

	static void DrawIndexed(
		const std::shared_ptr<D3D11VertexBuffer>& vertexBuffer,
		const std::shared_ptr<D3D11IndexBuffer>& indexBuffer)
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

	static void Begin()
	{
		s_RendererAPI->Begin();
	}

	static void End()
	{
		s_RendererAPI->End();
	}

	static void SetDepthFunc(const DepthFunc depthFunc)
	{
		s_RendererAPI->SetDepthFunc(depthFunc);
	}

	static void DrawPointList(const uint32_t count)
	{
		s_RendererAPI->DrawPointList(count);
	}

private:
	static D3D11RendererAPI* s_RendererAPI;
};
}
