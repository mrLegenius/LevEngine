#include "D3D11RendererAPI.h"

#include <iostream>
#include <wrl/client.h>

#include "d3d11.h"
extern ID3D11DeviceContext* context;

extern ID3D11RenderTargetView* rtv;
extern IDXGISwapChain* swapChain;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

ID3D11RenderTargetView* rtv;
ID3D11RasterizerState* rastState;

bool CreateRastState(ID3D11RasterizerState*& rastState)
{
	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	auto res = device->CreateRasterizerState(&rastDesc, &rastState);

	return SUCCEEDED(res);
}

void D3D11RendererAPI::Begin()
{
	context->ClearState();
	context->RSSetState(rastState);
	context->OMSetRenderTargets(1, &rtv, nullptr);
}

void D3D11RendererAPI::End()
{
	context->OMSetRenderTargets(0, nullptr, nullptr);

}

void D3D11RendererAPI::SetClearColor(float color[4])
{
	m_Color = color;
}

void D3D11RendererAPI::Clear()
{
	context->ClearRenderTargetView(rtv, m_Color);
}

void D3D11RendererAPI::DrawIndexed(
	const std::shared_ptr<D3D11VertexBuffer>& vertexBuffer,
	const std::shared_ptr<D3D11IndexBuffer>& indexBuffer)
{
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	indexBuffer->Bind();
	vertexBuffer->Bind();
	context->DrawIndexed(indexBuffer->GetCount(), 0, 0);
}

void D3D11RendererAPI::Init()
{
	ID3D11Texture2D* backTex;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backTex));	// __uuidof(ID3D11Texture2D)
	device->CreateRenderTargetView(backTex, nullptr, &rtv);

	if (!CreateRastState(rastState))
		std::cout << "Failed to create rast state";
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
