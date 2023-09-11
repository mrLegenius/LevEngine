#include "levpch.h"
#include <d3d11.h>
#include <wrl/client.h>

#include "D3D11RasterizerState.h"

namespace LevEngine
{
extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

inline D3D11_FILL_MODE ConvertFillMode(const FillMode fillMode)
{
    switch (fillMode)
    {
    case FillMode::Wireframe:
        return D3D11_FILL_WIREFRAME;
    case FillMode::Solid:
        return D3D11_FILL_SOLID;
    default:
        LEV_THROW("Unknown fill mode")
    	break;
    }
}

inline D3D11_CULL_MODE ConvertCullMode(const CullMode cullMode)
{
    switch (cullMode)
    {
    case CullMode::None:
        return D3D11_CULL_NONE;
    case CullMode::Front:
        return D3D11_CULL_FRONT;
    case CullMode::Back:
        return D3D11_CULL_BACK;
    case CullMode::FrontAndBack:
        // This mode is not supported in DX11.
        return D3D11_CULL_BACK;
    default:
        LEV_THROW("Unknown cull mode")
    	break;
    }
}

bool ConvertFrontFace(const FrontFace frontFace)
{
    switch (frontFace)
    {
    case FrontFace::Clockwise:
        return false;
    case FrontFace::CounterClockwise:
        return true;
    default:
        LEV_THROW("Unknown front face winding order")
        break;
    }
}

Vector<D3D11_RECT> TranslateRects(const Vector<Rect>& rects)
{
    Vector<D3D11_RECT> result(rects.size());
    for (unsigned int i = 0; i < rects.size(); i++)
    {
        auto& [left, top, right, bottom] = result[i];
        const Rect& rect = rects[i];

        top = static_cast<LONG>(rect.y + 0.5f);
        bottom = static_cast<LONG>(rect.y + rect.height + 0.5f);
        left = static_cast<LONG>(rect.x + 0.5f);
        right = static_cast<LONG>(rect.x + rect.width + 0.5f);
    }

    return result;
}

Vector<D3D11_VIEWPORT> TranslateViewports(const Vector<Viewport>& viewports)
{
    Vector<D3D11_VIEWPORT> result(viewports.size());
    for (unsigned int i = 0; i < viewports.size(); i++)
    {
        D3D11_VIEWPORT& d3dViewport = result[i];
        const Viewport& viewport = viewports[i];

        d3dViewport.TopLeftX = viewport.x;
        d3dViewport.TopLeftY = viewport.y;
        d3dViewport.Width = viewport.width;
        d3dViewport.Height = viewport.height;
        d3dViewport.MinDepth = viewport.minDepth;
        d3dViewport.MaxDepth = viewport.maxDepth;
    }

    return result;
}


D3D11RasterizerState::~D3D11RasterizerState()
{
    if (m_RasterizerState)
		m_RasterizerState->Release();
}

void D3D11RasterizerState::Bind()
{
    LEV_PROFILE_FUNCTION();

    if (m_StateDirty)
    {
        D3D11_RASTERIZER_DESC rasterizerDesc;
        rasterizerDesc.FillMode = ConvertFillMode(m_FrontFaceFillMode);
        rasterizerDesc.CullMode = ConvertCullMode(m_CullMode);
        rasterizerDesc.FrontCounterClockwise = ConvertFrontFace(m_FrontFace);
        rasterizerDesc.DepthBias = m_DepthBias < 0.0f ? static_cast<INT>(m_DepthBias - 0.5f) : static_cast<INT>(m_DepthBias + 0.5f);
        rasterizerDesc.DepthBiasClamp = m_BiasClamp;
        rasterizerDesc.SlopeScaledDepthBias = m_SlopeBias;
        rasterizerDesc.DepthClipEnable = m_DepthClipEnabled;
        rasterizerDesc.ScissorEnable = m_ScissorEnabled;
        rasterizerDesc.MultisampleEnable = m_MultisampleEnabled;
        rasterizerDesc.AntialiasedLineEnable = m_AntialiasedLineEnabled;

        if (m_RasterizerState)
            m_RasterizerState->Release();

        auto res = device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState);

        LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to create rasterizer state")

        m_StateDirty = false;
    }

    if (m_ScissorRectsDirty)
    {
        m_d3dRects = TranslateRects(m_ScissorRects);
        m_ScissorRectsDirty = false;
    }

    if (m_ViewportsDirty)
    {
        m_d3dViewports = TranslateViewports(m_Viewports);
        m_ViewportsDirty = false;
    }

    context->RSSetViewports(static_cast<UINT>(m_d3dViewports.size()), m_d3dViewports.data());
    context->RSSetScissorRects(static_cast<UINT>(m_d3dRects.size()), m_d3dRects.data());
    context->RSSetState(m_RasterizerState);
}

void D3D11RasterizerState::Unbind()
{
    LEV_PROFILE_FUNCTION();

    context->RSSetState(nullptr);
}
}