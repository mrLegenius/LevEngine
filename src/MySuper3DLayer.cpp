#include "MySuper3DLayer.h"

#include <directxmath.h>
#include <iostream>
#include <wrl/client.h>

#include "Application.h"
#include "D3D11IndexBuffer.h"
#include "Renderer/RenderCommand.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ID3D11DeviceContext* context;
Microsoft::WRL::ComPtr<ID3D11Device> device;
IDXGISwapChain* swapChain;
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

void MySuper3DLayer::OnAttach()
{
	ID3D11Texture2D* backTex;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backTex));	// __uuidof(ID3D11Texture2D)
	device->CreateRenderTargetView(backTex, nullptr, &rtv);
	m_Shader = std::make_unique<D3D11Shader>("./Shaders/MyVeryFirstShader.hlsl");

	DirectX::XMFLOAT4 points[8] =
	{
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),		DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),

		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),		DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};

	constexpr auto size = sizeof(DirectX::XMFLOAT4) * std::size(points);
	m_VertexBuffer = std::make_unique<D3D11VertexBuffer>(reinterpret_cast<float*>(points), size);

	const BufferLayout layout{
	{ ShaderDataType::Float4, "POSITION" },
	{ ShaderDataType::Float4, "COLOR" },
	};
	m_Shader->SetLayout(layout);
	m_VertexBuffer->SetLayout(layout);

	uint32_t indices[] = { 0,1,2, 1,0,3 };
	m_IndexBuffer = std::make_unique<D3D11IndexBuffer>(indices, std::size(indices));

	if (!CreateRastState(rastState))
		std::cout << "Failed to create rast state";

	context->RSSetState(rastState);
}

void MySuper3DLayer::OnUpdate()
{
	context->ClearState();

	context->RSSetState(rastState);

	const auto& window = Application::Get().GetWindow();
	RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());

	m_Shader->Bind();
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_IndexBuffer->Bind();
	m_VertexBuffer->Bind();

	context->OMSetRenderTargets(1, &rtv, nullptr);

	const float color[] = { 1.0f, 0.1f, 0.1f, 1.0f };
	context->ClearRenderTargetView(rtv, color);

	context->DrawIndexed(6, 0, 0);

	context->OMSetRenderTargets(0, nullptr, nullptr);
}
