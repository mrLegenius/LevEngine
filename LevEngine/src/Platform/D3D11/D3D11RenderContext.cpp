#include "levpch.h"
#include <d3d11_2.h>
#include <wrl/client.h>

#include "D3D11RefreshRateUtil.h"

#include "D3D11RenderContext.h"

#include "D3D11RenderDevice.h"
#include "Math/Math.h"
#include "Renderer/Pipeline/Texture.h"
#include "Platform/D3D11/D3D11Texture.h"
#include "Renderer/RenderDevice.h"
#include "Renderer/Pipeline/RenderTarget.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace LevEngine
{
    void D3D11RenderContext::Init(Ref<RenderDevice> renderDevice, const uint32_t width, const uint32_t height,
                                  bool isVSync, const HWND window)
    {
        LEV_PROFILE_FUNCTION();

        HRESULT res{};
        
        auto d3d11renderDevice = CastRef<D3D11RenderDevice>(renderDevice);
        m_DeviceContext = d3d11renderDevice->GetDeviceContext();
        
        Microsoft::WRL::ComPtr<IDXGIFactory2> factory;

        res = CreateDXGIFactory(__uuidof( IDXGIFactory2), &factory);
        LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to create DXGI factory.");

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc = {};

        swapChainFullScreenDesc.RefreshRate = QueryRefreshRate(width, height, isVSync);
        swapChainFullScreenDesc.Windowed = true;

        // First create a DXGISwapChain1
        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

        res = factory->CreateSwapChainForHwnd(d3d11renderDevice->GetDevice().Get(), window,
                                              &swapChainDesc, &swapChainFullScreenDesc, nullptr, &swapChain);
        LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to create swap chain.");

        // Now query for the IDXGISwapChain2 interface.
        res = swapChain.Get()->QueryInterface<IDXGISwapChain2>(&m_SwapChain);
        LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to retrieve IDXGISwapChain2 interface.");

        res = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_BackBuffer);
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

    void D3D11RenderContext::SwapBuffers(bool isVSync)
    {
        LEV_PROFILE_FUNCTION();

        m_RenderTarget->Bind();

        // Copy the render target's color buffer to the swap chain's back buffer.
        const Ref<D3D11Texture> colorBuffer = CastRef<D3D11Texture>(m_RenderTarget->GetTexture(AttachmentPoint::Color0));
        if (colorBuffer)
            m_DeviceContext->CopyResource(m_BackBuffer.Get(), colorBuffer->GetTextureResource());

        m_SwapChain->Present(isVSync ? 1 : 0, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
    }

    void D3D11RenderContext::ResizeBackBuffer(uint16_t width, uint16_t height)
    {
        // If either the width or the height are 0, make them 1.
        width = Math::Max<uint32_t>(width, 1u);
        height = Math::Max<uint32_t>(height, 1u);

        //// Make sure we're not referencing the render targets when the window is resized.
        m_DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

        // Release the current render target views and texture resources.
        m_BackBuffer.Reset();

        // Resize the swap chain buffers.
        {
            const auto res = m_SwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
            LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to resize the swap chain buffer");
        }
        {
            const auto res = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_BackBuffer);
            LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to get back buffer pointer from swap chain");
        }

        m_RenderTarget->Resize(width, height);
    }
}
