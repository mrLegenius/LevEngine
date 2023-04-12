#include "D3D11Context.h"

#include <cstdint>
#include <d3d11.h>
#include <iostream>
#include <wrl/client.h>

#include "D3D11Texture.h"
#include "Debugging/Profiler.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ID3D11DeviceContext* context;
Microsoft::WRL::ComPtr<ID3D11Device> device;
IDXGISwapChain* swapChain;

D3D11Context::~D3D11Context()
{
	m_BackBuffer->Release();
}

void D3D11Context::Init(uint32_t width, uint32_t height, HWND window)
{
	constexpr D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.Height = height;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = window;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	const auto res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_SINGLETHREADED,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&device,
		nullptr,
		&context);

	if (FAILED(res))
		std::cout << "Failed to create device and swap chain";

	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_BackBuffer));

	Texture::TextureFormat depthStencilTextureFormat(
		Texture::Components::DepthStencil,
		Texture::Type::UnsignedNormalized,
		1,
		0, 0, 0, 0, 24, 8);
	std::shared_ptr<Texture> depthStencilTexture = D3D11Texture::CreateTexture2D(width, height, 1, depthStencilTextureFormat);

	// Color buffer (Color0)
	Texture::TextureFormat colorTextureFormat(
		Texture::Components::RGBA,
		Texture::Type::UnsignedNormalized,
		1,
		8, 8, 8, 8, 0, 0);
	std::shared_ptr<Texture> colorTexture = D3D11Texture::CreateTexture2D(width, height, 1, colorTextureFormat);
	m_RenderTarget = CreateRef<D3D11RenderTarget>();
	m_RenderTarget->AttachTexture(AttachmentPoint::Color0, colorTexture);
	m_RenderTarget->AttachTexture(AttachmentPoint::DepthStencil, depthStencilTexture);
}

void D3D11Context::SwapBuffers()
{
	LEV_PROFILE_FUNCTION();

	m_RenderTarget->Bind();

	// Copy the render target's color buffer to the swap chain's back buffer.
	Ref<D3D11Texture> colorBuffer = std::dynamic_pointer_cast<D3D11Texture>(m_RenderTarget->GetTexture(AttachmentPoint::Color0));
	if (colorBuffer)
		context->CopyResource(m_BackBuffer, colorBuffer->GetTextureResource());

	swapChain->Present(0, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}
