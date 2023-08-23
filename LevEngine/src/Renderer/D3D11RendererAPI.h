#pragma once

#include "IndexBuffer.h"
#include "VertexBuffer.h"

namespace LevEngine
{
class D3D11RendererAPI
{
public:
	~D3D11RendererAPI() = default;

	void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer);
	void DrawFullScreenQuad();
	//void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;

	//void SetLineWidth(float width);

	void DrawPointList(uint32_t count);
};
}
