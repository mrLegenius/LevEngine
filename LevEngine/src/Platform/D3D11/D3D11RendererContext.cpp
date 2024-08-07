#include "levpch.h"
#include <d3d11.h>
#include <wrl/client.h>

#include "D3D11RendererContext.h"

#include "Math/Math.h"
#include "Renderer/Texture.h"
#include "Platform/D3D11/D3D11Texture.h"
#include "Renderer/RenderTarget.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace LevEngine
{
ID3D11DeviceContext* context;
Microsoft::WRL::ComPtr<ID3D11Device> device;
IDXGISwapChain* swapChain;

D3D11RendererContext::~D3D11RendererContext()
{
	LEV_PROFILE_FUNCTION();

	if (m_BackBuffer) m_BackBuffer->Release();
	if (context) context->Release();
	if (device) device->Release();
	if (swapChain) swapChain->Release();
}

void D3D11RendererContext::Init(const uint32_t width, const uint32_t height, const HWND window)
{
	LEV_PROFILE_FUNCTION();

	constexpr D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC swapDesc;
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

	auto res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&device,
		nullptr,
		&context);

	LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to create device and swap chain")
	
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_BackBuffer));
	LEV_CORE_ASSERT(SUCCEEDED(res), "Unable to get buffer from swap chain")

	const Texture::TextureFormat depthStencilTextureFormat(
		Texture::Components::DepthStencil,
		Texture::Type::UnsignedNormalized,
		1,
		0, 0, 0, 0, 24, 8);
	const Ref<Texture> depthStencilTexture = Texture::CreateTexture2D(width, height, 1, depthStencilTextureFormat);

	// Color buffer (Color0)
	const Texture::TextureFormat colorTextureFormat(
		Texture::Components::RGBA,
		Texture::Type::UnsignedNormalized,
		1,
		8, 8, 8, 8, 0, 0);
	const Ref<Texture> colorTexture = Texture::CreateTexture2D(width, height, 1, colorTextureFormat);
	m_RenderTarget = RenderTarget::Create();
	m_RenderTarget->AttachTexture(AttachmentPoint::Color0, colorTexture);
	m_RenderTarget->AttachTexture(AttachmentPoint::DepthStencil, depthStencilTexture);
}

void D3D11RendererContext::SwapBuffers()
{
	LEV_PROFILE_FUNCTION();

	m_RenderTarget->Bind();

	// Copy the render target's color buffer to the swap chain's back buffer.
	const Ref<D3D11Texture> colorBuffer = CastRef<D3D11Texture>(m_RenderTarget->GetTexture(AttachmentPoint::Color0));
	if (colorBuffer)
		context->CopyResource(m_BackBuffer, colorBuffer->GetTextureResource());

	swapChain->Present(0, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void D3D11RendererContext::ResizeBackBuffer(uint16_t width, uint16_t height)
{
	// If either the width or the height are 0, make them 1.
	width = Math::Max<uint32_t>(width, 1u);
	height = Math::Max<uint32_t>(height, 1u);

	//// Make sure we're not referencing the render targets when the window is resized.
	context->OMSetRenderTargets(0, nullptr, nullptr);

	// Release the current render target views and texture resources.
	if (m_BackBuffer) m_BackBuffer->Release();

	// Resize the swap chain buffers.
	{
		const auto res = swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to resize the swap chain buffer");
	}
	{
		const auto res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_BackBuffer));
		LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to get back buffer pointer from swap chain");
	}

	m_RenderTarget->Resize(width, height);
}
}
