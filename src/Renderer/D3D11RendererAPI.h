#pragma once
#include <cstdint>

class D3D11RendererAPI
{
public:
	~D3D11RendererAPI() = default;

	void SetClearColor(const float color[4]);
	void Clear();

	//void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
	//void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;

	void SetLineWidth(float width);

	void Init();
	void SetViewport(int x, int y, uint32_t width, uint32_t height);
	//void SetDepthFunc(DepthFunc depthFunc) = 0;
};
