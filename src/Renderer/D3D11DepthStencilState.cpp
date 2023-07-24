#include "pch.h"
#include "D3D11DepthStencilState.h"

#include <cassert>
#include <wrl/client.h>

#include "Debugging/Profiler.h"
namespace LevEngine
{
extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

inline D3D11_DEPTH_WRITE_MASK ConvertDepthWriteMask(const DepthWrite depthWrite)
{
    D3D11_DEPTH_WRITE_MASK result = D3D11_DEPTH_WRITE_MASK_ALL;

    switch (depthWrite)
    {
    case DepthWrite::Enable:
        result = D3D11_DEPTH_WRITE_MASK_ALL;
        break;
    case DepthWrite::Disable:
        result = D3D11_DEPTH_WRITE_MASK_ZERO;
        break;
    default:
        assert(false && "Unknown depth write mask");
        break;
    }

    return result;
}

inline D3D11_COMPARISON_FUNC ConvertCompareFunc(const CompareFunction compareFunc)
{
    D3D11_COMPARISON_FUNC result = D3D11_COMPARISON_LESS;

    switch (compareFunc)
    {
    case CompareFunction::Never:
        result = D3D11_COMPARISON_NEVER;
        break;
    case CompareFunction::Less:
        result = D3D11_COMPARISON_LESS;
        break;
    case CompareFunction::Equal:
        result = D3D11_COMPARISON_EQUAL;
        break;
    case CompareFunction::LessOrEqual:
        result = D3D11_COMPARISON_LESS_EQUAL;
        break;
    case CompareFunction::Greater:
        result = D3D11_COMPARISON_GREATER;
        break;
    case CompareFunction::NotEqual:
        result = D3D11_COMPARISON_NOT_EQUAL;
        break;
    case CompareFunction::GreaterOrEqual:
        result = D3D11_COMPARISON_GREATER_EQUAL;
        break;
    case CompareFunction::Always:
        result = D3D11_COMPARISON_ALWAYS;
        break;
    default:
        assert(false && "Unknown compare function");
        break;
    }

    return result;
}

inline D3D11_STENCIL_OP ConvertStencilOperation(const StencilOperation stencilOperation)
{
    D3D11_STENCIL_OP result = D3D11_STENCIL_OP_KEEP;

    switch (stencilOperation)
    {
    case StencilOperation::Keep:
        result = D3D11_STENCIL_OP_KEEP;
        break;
    case StencilOperation::Zero:
        result = D3D11_STENCIL_OP_ZERO;
        break;
    case StencilOperation::Reference:
        result = D3D11_STENCIL_OP_REPLACE;
        break;
    case StencilOperation::IncrementClamp:
        result = D3D11_STENCIL_OP_INCR_SAT;
        break;
    case StencilOperation::DecrementClamp:
        result = D3D11_STENCIL_OP_DECR_SAT;
        break;
    case StencilOperation::Invert:
        result = D3D11_STENCIL_OP_INVERT;
        break;
    case StencilOperation::IncrementWrap:
        result = D3D11_STENCIL_OP_INCR;
        break;
    case StencilOperation::DecrementWrap:
        result = D3D11_STENCIL_OP_DECR;
        break;
    default:
        assert(false && "Unknown stencil operation");
        break;
    }

    return result;
}

inline D3D11_DEPTH_STENCILOP_DESC ConvertFaceOperation(const FaceOperation faceOperation)
{
    D3D11_DEPTH_STENCILOP_DESC result;

    result.StencilFailOp = ConvertStencilOperation(faceOperation.StencilFail);
    result.StencilDepthFailOp = ConvertStencilOperation(faceOperation.StencilPassDepthFail);
    result.StencilPassOp = ConvertStencilOperation(faceOperation.StencilDepthPass);
    result.StencilFunc = ConvertCompareFunc(faceOperation.StencilFunction);

    return result;
}

D3D11DepthStencilState::~D3D11DepthStencilState()
{
    if (m_DepthStencilState)
        m_DepthStencilState->Release();
}

void D3D11DepthStencilState::Bind()
{
    LEV_PROFILE_FUNCTION();

    if (m_Dirty)
    {
        D3D11_DEPTH_STENCIL_DESC desc;

        desc.DepthEnable = m_DepthMode.DepthEnable;
        desc.DepthWriteMask = ConvertDepthWriteMask(m_DepthMode.DepthWriteMask);
        desc.DepthFunc = ConvertCompareFunc(m_DepthMode.DepthFunction);
        desc.StencilEnable = m_StencilMode.StencilEnabled;
        desc.StencilReadMask = m_StencilMode.ReadMask;
        desc.StencilWriteMask = m_StencilMode.WriteMask;
        desc.FrontFace = ConvertFaceOperation(m_StencilMode.FrontFace);
        desc.BackFace = ConvertFaceOperation(m_StencilMode.BackFace);

        if (m_DepthStencilState)
            m_DepthStencilState->Release();

        if (FAILED(device->CreateDepthStencilState(&desc, &m_DepthStencilState)))
	        assert(false && "Failed to create depth stencil state.");

        m_Dirty = false;
    }

    context->OMSetDepthStencilState(m_DepthStencilState, m_StencilMode.StencilReference);
}

void D3D11DepthStencilState::Unbind()
{
    LEV_PROFILE_FUNCTION();

    context->OMSetDepthStencilState(nullptr, m_StencilMode.StencilReference);
}
}