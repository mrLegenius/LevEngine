#include "pch.h"
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
        break;
    case FillMode::Solid:
        return D3D11_FILL_SOLID;
    default:
        LEV_THROW("Unknown fill mode")
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
    }
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
        //TODO: DirectX 11.1 support
        //rasterizerDesc.DepthBias = (m_DepthBias < 0.0f) ? static_cast<INT>(m_DepthBias - 0.5f) : static_cast<INT>(m_DepthBias + 0.5f);
        //rasterizerDesc.DepthBiasClamp = m_BiasClamp;
        //rasterizerDesc.SlopeScaledDepthBias = m_SlopeBias;
        rasterizerDesc.DepthClipEnable = m_DepthClipEnabled;
        //rasterizerDesc.ScissorEnable = m_ScissorEnabled;
        //rasterizerDesc.MultisampleEnable = m_MultisampleEnabled;
        //rasterizerDesc.AntialiasedLineEnable = m_AntialiasedLineEnabled;

        if (m_RasterizerState)
            m_RasterizerState->Release();

        auto res = device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState);

        LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to create rasterizer state")

        m_StateDirty = false;
    }

    context->RSSetState(m_RasterizerState);
}

void D3D11RasterizerState::Unbind()
{
    LEV_PROFILE_FUNCTION();

    context->RSSetState(nullptr);
}
}