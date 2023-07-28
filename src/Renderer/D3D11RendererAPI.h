#pragma once

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "DepthFunc.h"

namespace LevEngine
{
class D3D11RendererAPI
{
public:
	~D3D11RendererAPI() = default;

	void Begin();
	void End();
	void SetClearColor(float color[4]);
	void Clear();
	void SetDepthFunc(DepthFunc depthFunc);

	void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer);
	void DrawFullScreenQuad();
	//void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;

	void SetLineWidth(float width);

	void Init();
	void SetViewport(int x, int y, uint32_t width, uint32_t height);
	void DrawPointList(uint32_t count);

private:
	float* m_Color;
};
}
