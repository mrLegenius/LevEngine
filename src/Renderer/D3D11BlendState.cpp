#include "pch.h"
#include "D3D11BlendState.h"

#include <cassert>
#include <wrl/client.h>

#include "Debugging/Profiler.h"
namespace LevEngine
{
extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

BlendMode BlendMode::AlphaBlending = BlendMode{ true, false, BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha };
BlendMode BlendMode::Additive = BlendMode{ true, false, BlendFactor::One, BlendFactor::One };

D3D11_BLEND ConvertBlendFactor(const BlendFactor blendFactor)
{
    D3D11_BLEND result = D3D11_BLEND_ONE;

    switch (blendFactor)
    {
    case BlendFactor::Zero:
        result = D3D11_BLEND_ZERO;
        break;
    case BlendFactor::One:
        result = D3D11_BLEND_ONE;
        break;
    case BlendFactor::SrcColor:
        result = D3D11_BLEND_SRC_COLOR;
        break;
    case BlendFactor::OneMinusSrcColor:
        result = D3D11_BLEND_INV_SRC_COLOR;
        break;
    case BlendFactor::DstColor:
        result = D3D11_BLEND_DEST_COLOR;
        break;
    case BlendFactor::OneMinusDstColor:
        result = D3D11_BLEND_INV_DEST_COLOR;
        break;
    case BlendFactor::SrcAlpha:
        result = D3D11_BLEND_SRC_ALPHA;
        break;
    case BlendFactor::OneMinusSrcAlpha:
        result = D3D11_BLEND_INV_SRC_ALPHA;
        break;
    case BlendFactor::DstAlpha:
        result = D3D11_BLEND_DEST_ALPHA;
        break;
    case BlendFactor::OneMinusDstAlpha:
        result = D3D11_BLEND_INV_DEST_ALPHA;
        break;
    case BlendFactor::SrcAlphaSat:
        result = D3D11_BLEND_SRC_ALPHA_SAT;
        break;
    case BlendFactor::ConstBlendFactor:
        result = D3D11_BLEND_BLEND_FACTOR;
        break;
    case BlendFactor::OneMinusBlendFactor:
        result = D3D11_BLEND_INV_BLEND_FACTOR;
        break;
    case BlendFactor::Src1Color:
        result = D3D11_BLEND_SRC1_COLOR;
        break;
    case BlendFactor::OneMinusSrc1Color:
        result = D3D11_BLEND_INV_SRC1_COLOR;
        break;
    case BlendFactor::Src1Alpha:
        result = D3D11_BLEND_INV_SRC1_ALPHA;
        break;
    case BlendFactor::OneMinusSrc1Alpha:
        result = D3D11_BLEND_INV_SRC1_ALPHA;
        break;
    default:
        LEV_THROW("Unknown blend factor")
        break;
    }

    return result;
}

D3D11_BLEND_OP ConvertBlendOp(BlendOperation blendOperation)
{
    D3D11_BLEND_OP result = D3D11_BLEND_OP_ADD;
    switch (blendOperation)
    {
    case BlendOperation::Add:
        result = D3D11_BLEND_OP_ADD;
        break;
    case BlendOperation::Subtract:
        result = D3D11_BLEND_OP_SUBTRACT;
        break;
    case BlendOperation::ReverseSubtract:
        result = D3D11_BLEND_OP_REV_SUBTRACT;
        break;
    case BlendOperation::Min:
        result = D3D11_BLEND_OP_MIN;
        break;
    case BlendOperation::Max:
        result = D3D11_BLEND_OP_MAX;
        break;
    default:
        LEV_THROW("Unknown blend operation")
        break;
    }

    return result;
}

D3D11_COLOR_WRITE_ENABLE ConvertWriteMask(BlendWrite blendWrite)
{
	const auto writeMask = static_cast<D3D11_COLOR_WRITE_ENABLE>(blendWrite);
    return writeMask;
}

//TODO: Direct3D 11.1 Support
//D3D11_LOGIC_OP ConvertLogicOperator(LogicOperator logicOp)
//{
//    D3D11_LOGIC_OP result = D3D11_LOGIC_OP_NOOP;
//
//    switch (logicOp)
//    {
//    case LogicOperator::None:
//        result = D3D11_LOGIC_OP_NOOP;
//        break;
//    case LogicOperator::Clear:
//        result = D3D11_LOGIC_OP_CLEAR;
//        break;
//    case LogicOperator::Set:
//        result = D3D11_LOGIC_OP_SET;
//        break;
//    case LogicOperator::Copy:
//        result = D3D11_LOGIC_OP_SET;
//        break;
//    case LogicOperator::CopyInverted:
//        result = D3D11_LOGIC_OP_COPY_INVERTED;
//        break;
//    case LogicOperator::Invert:
//        result = D3D11_LOGIC_OP_INVERT;
//        break;
//    case LogicOperator::And:
//        result = D3D11_LOGIC_OP_AND;
//        break;
//    case LogicOperator::Nand:
//        result = D3D11_LOGIC_OP_NAND;
//        break;
//    case LogicOperator::Or:
//        result = D3D11_LOGIC_OP_OR;
//        break;
//    case LogicOperator::Nor:
//        result = D3D11_LOGIC_OP_NOR;
//        break;
//    case LogicOperator::Xor:
//        result = D3D11_LOGIC_OP_XOR;
//        break;
//    case LogicOperator::Equiv:
//        result = D3D11_LOGIC_OP_EQUIV;
//        break;
//    case LogicOperator::AndReverse:
//        result = D3D11_LOGIC_OP_AND_REVERSE;
//        break;
//    case LogicOperator::AndInverted:
//        result = D3D11_LOGIC_OP_AND_INVERTED;
//        break;
//    case LogicOperator::OrReverse:
//        result = D3D11_LOGIC_OP_OR_REVERSE;
//        break;
//    case LogicOperator::OrInverted:
//        result = D3D11_LOGIC_OP_OR_INVERTED;
//        break;
//    default:
//        break;
//    }
//
//    return result;
//}

D3D11BlendState::~D3D11BlendState()
{
    if (m_BlendState)
		m_BlendState->Release();
}

void D3D11BlendState::Bind()
{
    LEV_PROFILE_FUNCTION();

    if (m_Dirty)
    {
        D3D11_BLEND_DESC blendDesc;

        blendDesc.AlphaToCoverageEnable = m_AlphaToCoverageEnabled;
        blendDesc.IndependentBlendEnable = m_IndependentBlendEnabled;
        for (unsigned int i = 0; i < 8 && i < m_BlendModes.size(); i++)
        {
            D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[i];
            BlendMode& blendMode = m_BlendModes[i];

            rtBlendDesc.BlendEnable = blendMode.BlendEnabled;
            //rtBlendDesc.LogicOpEnable = blendMode.LogicOpEnabled; //TODO: Direct3D 11.1 Support
            rtBlendDesc.SrcBlend = ConvertBlendFactor(blendMode.SrcFactor);
            rtBlendDesc.DestBlend = ConvertBlendFactor(blendMode.DstFactor);
            rtBlendDesc.BlendOp = ConvertBlendOp(blendMode.BlendOp);
            rtBlendDesc.SrcBlendAlpha = ConvertBlendFactor(blendMode.SrcAlphaFactor);
            rtBlendDesc.DestBlendAlpha = ConvertBlendFactor(blendMode.DstAlphaFactor);
            rtBlendDesc.BlendOpAlpha = ConvertBlendOp(blendMode.AlphaOp);
            //rtBlendDesc.LogicOp = ConvertLogicOperator(blendMode.LogicOp); //TODO: Direct3D 11.1 Support
            rtBlendDesc.RenderTargetWriteMask = ConvertWriteMask(blendMode.Write);
        }

        if (m_BlendState)
            m_BlendState->Release();

        device->CreateBlendState(&blendDesc, &m_BlendState);

        m_Dirty = false;
    }

    context->OMSetBlendState(m_BlendState, &m_ConstBlendFactor.x, m_SampleMask);
}

void D3D11BlendState::Unbind()
{
    LEV_PROFILE_FUNCTION();

    context->OMSetBlendState(nullptr, &m_ConstBlendFactor.x, m_SampleMask);
}
}