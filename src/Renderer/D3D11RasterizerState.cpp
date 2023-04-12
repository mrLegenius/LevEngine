#include "D3D11RasterizerState.h"

#include <cassert>
#include <d3d11.h>
#include <wrl/client.h>

#include "Debugging/Profiler.h"

extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

inline D3D11_FILL_MODE ConvertFillMode(const FillMode fillMode)
{
    D3D11_FILL_MODE result = D3D11_FILL_SOLID;
    switch (fillMode)
    {
    case FillMode::Wireframe:
        result = D3D11_FILL_WIREFRAME;
        break;
    case FillMode::Solid:
        result = D3D11_FILL_SOLID;
        break;
    default:
        assert(false && "Unknown fill mode.");
        break;
    }

    return result;
}

inline D3D11_CULL_MODE ConvertCullMode(const CullMode cullMode)
{
    D3D11_CULL_MODE result = D3D11_CULL_BACK;
    switch (cullMode)
    {
    case CullMode::None:
        result = D3D11_CULL_NONE;
        break;
    case CullMode::Front:
        result = D3D11_CULL_FRONT;
        break;
    case CullMode::Back:
        result = D3D11_CULL_BACK;
        break;
    case CullMode::FrontAndBack:
        // This mode is not supported in DX11.
        break;
    default:
        assert(false && "Unknown cull mode.");
        break;
    }

    return result;
}

bool ConvertFrontFace(const FrontFace frontFace)
{
    bool frontCounterClockwise = true;
    switch (frontFace)
    {
    case FrontFace::Clockwise:
        frontCounterClockwise = false;
        break;
    case FrontFace::CounterClockwise:
        frontCounterClockwise = true;
        break;
    default:
        assert(false && "Unknown front face winding order.");
        break;
    }

    return frontCounterClockwise;
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
        D3D11_RASTERIZER_DESC rasterizerDesc = {};

        rasterizerDesc.FillMode = ConvertFillMode(m_FrontFaceFillMode);
        rasterizerDesc.CullMode = ConvertCullMode(m_CullMode);
        rasterizerDesc.FrontCounterClockwise = ConvertFrontFace(m_FrontFace);
        //rasterizerDesc.DepthBias = (m_DepthBias < 0.0f) ? static_cast<INT>(m_DepthBias - 0.5f) : static_cast<INT>(m_DepthBias + 0.5f);
        //rasterizerDesc.DepthBiasClamp = m_BiasClamp;
        //rasterizerDesc.SlopeScaledDepthBias = m_SlopeBias;
        rasterizerDesc.DepthClipEnable = m_DepthClipEnabled;
        //rasterizerDesc.ScissorEnable = m_ScissorEnabled;
        //rasterizerDesc.MultisampleEnable = m_MultisampleEnabled;
        //rasterizerDesc.AntialiasedLineEnable = m_AntialiasedLineEnabled;

        if (m_RasterizerState)
            m_RasterizerState->Release();

        if (FAILED(device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState)))
	        assert(false && "Failed to create rasterizer state.");

        m_StateDirty = false;
    }

    context->RSSetState(m_RasterizerState);
}

void D3D11RasterizerState::Unbind()
{
    LEV_PROFILE_FUNCTION();

    context->RSSetState(nullptr);
}
