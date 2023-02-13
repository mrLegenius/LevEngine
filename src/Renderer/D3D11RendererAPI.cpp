#include "D3D11RendererAPI.h"
#include "d3d11.h"
extern ID3D11DeviceContext* context;
void D3D11RendererAPI::SetClearColor(const float color[4])
{
}

void D3D11RendererAPI::Clear()
{
}

void D3D11RendererAPI::SetLineWidth(float width)
{
}

void D3D11RendererAPI::Init()
{
}

void D3D11RendererAPI::SetViewport(int x, int y, uint32_t width, uint32_t height)
{
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
}
