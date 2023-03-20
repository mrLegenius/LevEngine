#pragma once
#include <cstdint>
#include <memory>

#include "D3D11IndexBuffer.h"
#include "D3D11VertexBuffer.h"
#include "DepthFunc.h"

class D3D11RendererAPI
{
public:
	~D3D11RendererAPI() = default;

	void Begin();
	void End();
	void SetClearColor(float color[4]);
	void Clear();
	void SetDepthFunc(DepthFunc depthFunc);

	void DrawIndexed(const std::shared_ptr<D3D11VertexBuffer>& vertexBuffer, const std::shared_ptr<D3D11IndexBuffer>& indexBuffer);
	//void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;

	void SetLineWidth(float width);

	void Init();
	void SetViewport(int x, int y, uint32_t width, uint32_t height);
private:
	float* m_Color;
};
