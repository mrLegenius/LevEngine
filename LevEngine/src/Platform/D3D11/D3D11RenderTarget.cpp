#include "levpch.h"
#include <d3d11.h>
#include <wrl/client.h>


#include "D3D11RenderTarget.h"

#include "D3D11StructuredBuffer.h"
#include "D3D11Texture.h"

namespace LevEngine
{
extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

inline Ref<D3D11Texture> ConvertTextureToD3D11Texture(const Ref<Texture>& texture)
{
    return CastRef<D3D11Texture>(texture);
}

void D3D11RenderTarget::Bind()
{
    LEV_PROFILE_FUNCTION();

    if (m_CheckValidity)
    {
        LEV_CORE_ASSERT(IsValid(), "Invalid render target")
        m_CheckValidity = false;
    }

    ID3D11RenderTargetView* renderTargetViews[8];
    UINT numRTVs = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
	    if (const Ref<D3D11Texture> texture = ConvertTextureToD3D11Texture(m_Textures[i]))
		    renderTargetViews[numRTVs++] = texture->GetRenderTargetView();
    }
    
    ID3D11UnorderedAccessView* uavViews[8];
    const UINT uavStartSlot = numRTVs;
    UINT numUAVs = 0;

    for (uint8_t i = 0; i < StructuredBuffersCount; i++)
    {
        if (Ref<D3D11StructuredBuffer> rwBuffer = CastRef<D3D11StructuredBuffer>(m_StructuredBuffers[i]))
	        uavViews[numUAVs++] = rwBuffer->GetUnorderedAccessView();
    }

    ID3D11DepthStencilView* depthStencilView = nullptr;
    const Ref<D3D11Texture> depthTexture = ConvertTextureToD3D11Texture(m_Textures[static_cast<uint8_t>(AttachmentPoint::Depth)]);
    const Ref<D3D11Texture> depthStencilTexture = ConvertTextureToD3D11Texture(m_Textures[static_cast<uint8_t>(AttachmentPoint::DepthStencil)]);

    if (depthTexture)
    {
        depthStencilView = depthTexture->GetDepthStencilView();
    }
    else if (depthStencilTexture)
    {
        depthStencilView = depthStencilTexture->GetDepthStencilView();
    }

    context->OMSetRenderTargetsAndUnorderedAccessViews(numRTVs, renderTargetViews, depthStencilView, uavStartSlot, numUAVs, uavViews, nullptr);
}

void D3D11RenderTarget::Unbind()
{
    LEV_PROFILE_FUNCTION();

    context->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 0, nullptr, nullptr);
}

bool D3D11RenderTarget::IsValid() const
{
    UINT numRTV = 0;
    int width = -1;
    int height = -1;

    for (auto texture : m_Textures)
    {
        if (texture)
        {
            if (ConvertTextureToD3D11Texture(texture)->GetRenderTargetView()) 
                ++numRTV;

            if (width == -1 || height == -1)
            {
                width = texture->GetWidth();
                height = texture->GetHeight();
            }
            else
            {
                if (texture->GetWidth() != width || texture->GetHeight() != height)
                {
                    return false;
                }
            }
        }
    }

    UINT numUAV = 0;
    for (auto rwBuffer : m_StructuredBuffers)
    {
        if (rwBuffer)
        {
            ++numUAV;
        }
    }

    if (numRTV + numUAV > 8)
    {
        return false;
    }

    return true;
}
}