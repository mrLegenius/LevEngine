#include "D3D11RendererAPI.h"

#include <cassert>
#include <iostream>
#include <wrl/client.h>

#include "d3d11.h"
#include "../Kernel/Application.h"
extern ID3D11DeviceContext* context;

extern IDXGISwapChain* swapChain;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

ID3D11RenderTargetView* rtv;
ID3D11DepthStencilView* dsv;
ID3D11RasterizerState* rastState;

static D3D11_COMPARISON_FUNC ConvertDepthFuncToGl(DepthFunc depthFunc)
{
	switch (depthFunc)
	{
	case DepthFunc::Never:
		return D3D11_COMPARISON_NEVER;
	case DepthFunc::Less:
		return D3D11_COMPARISON_LESS;
	case DepthFunc::LessOrEqual:
		return D3D11_COMPARISON_LESS_EQUAL;
	case DepthFunc::Equal:
		return D3D11_COMPARISON_EQUAL;
	case DepthFunc::NotEqual:
		return D3D11_COMPARISON_NOT_EQUAL;
	case DepthFunc::GreaterOrEqual:
		return D3D11_COMPARISON_GREATER_EQUAL;
	case DepthFunc::Greater:
		return D3D11_COMPARISON_GREATER;
	case DepthFunc::Always:
		return D3D11_COMPARISON_ALWAYS;
	default:
		assert(false && "[OpenGL Renderer API] Unknown depth function");
	}
}

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
	context->OMSetRenderTargets(1, &rtv, dsv);
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
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void D3D11RendererAPI::SetDepthFunc(DepthFunc depthFunc)
{

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

	D3D11_DEPTH_STENCIL_DESC dsDesc{ };
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = ConvertDepthFuncToGl(DepthFunc::Less);
	dsDesc.StencilEnable = false;

	ID3D11DepthStencilState* pDSState;
	device->CreateDepthStencilState(&dsDesc, &pDSState);

	context->OMSetDepthStencilState(pDSState, 1u);
	
	ID3D11Texture2D* pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth{};
	descDepth.Width = Application::Get().GetWindow().GetWidth();
	descDepth.Height = Application::Get().GetWindow().GetHeight();
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	device->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV{};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;

	device->CreateDepthStencilView(pDepthStencil, &descDSV, &dsv);
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
