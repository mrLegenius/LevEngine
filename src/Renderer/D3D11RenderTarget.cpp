#include "pch.h"
#include "D3D11RenderTarget.h"

#include <wrl/client.h>

#include "Debugging/Profiler.h"
namespace LevEngine
{
extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

void D3D11RenderTarget::Bind()
{
    LEV_PROFILE_FUNCTION();

    if (m_CheckValidity)
    {
        if (!IsValid())
        {
            assert(false && "Invalid render target");
        }
        m_CheckValidity = false;
    }

    ID3D11RenderTargetView* renderTargetViews[8];
    UINT numRTVs = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
	    if (const std::shared_ptr<D3D11Texture> texture = m_Textures[i])
		    renderTargetViews[numRTVs++] = texture->GetRenderTargetView();
    }

    /* //TODO: UAV Support
    ID3D11UnorderedAccessView* uavViews[8];
    const UINT uavStartSlot = numRTVs;
    UINT numUAVs = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        std::shared_ptr<StructuredBufferDX11> rwbuffer = m_StructuredBuffers[i];
        if (rwbuffer)
	        uavViews[numUAVs++] = rwbuffer->GetUnorderedAccessView();
    }
    */

    ID3D11DepthStencilView* depthStencilView = nullptr;
    std::shared_ptr<D3D11Texture> depthTexture = m_Textures[(uint8_t)AttachmentPoint::Depth];
    std::shared_ptr<D3D11Texture> depthStencilTexture = m_Textures[(uint8_t)AttachmentPoint::DepthStencil];

    if (depthTexture)
    {
        depthStencilView = depthTexture->GetDepthStencilView();
    }
    else if (depthStencilTexture)
    {
        depthStencilView = depthStencilTexture->GetDepthStencilView();
    }

	context->OMSetRenderTargets(numRTVs, renderTargetViews, depthStencilView);
    //context->OMSetRenderTargetsAndUnorderedAccessViews(numRTVs, renderTargetViews, depthStencilView, uavStartSlot, numUAVs, uavViews, nullptr); //TODO: UAV Support
}

void D3D11RenderTarget::Unbind()
{
    LEV_PROFILE_FUNCTION();

    context->OMSetRenderTargets(0, nullptr, nullptr);
    //context->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 0, nullptr, nullptr); //TODO: UAV Support
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
            if (texture->GetRenderTargetView()) ++numRTV;

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
    //TODO: UAV Support
    /*for (auto rwBuffer : m_StructuredBuffers)
    {
        if (rwBuffer)
        {
            ++numUAV;
        }
    }*/

    if (numRTV + numUAV > 8)
    {
        return false;
    }

    return true;
}
}