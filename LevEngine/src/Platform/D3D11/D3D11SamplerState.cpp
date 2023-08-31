#include "levpch.h"
#include <wrl/client.h>

#include "D3D11SamplerState.h"

namespace LevEngine
{
	extern ID3D11DeviceContext* context;
	extern Microsoft::WRL::ComPtr<ID3D11Device> device;

	D3D11SamplerState::~D3D11SamplerState()
	{
		if (m_SamplerState)
			m_SamplerState->Release();
	}

	D3D11_FILTER D3D11SamplerState::TranslateFilter() const
	{
		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		if (m_IsAnisotropicFilteringEnabled)
		{
			filter = (m_CompareMode == CompareMode::None) ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_COMPARISON_ANISOTROPIC;
			return filter;
		}

		if (m_MinFilter == MinFilter::Nearest && m_MagFilter == MagFilter::Nearest && m_MipFilter == MipFilter::Nearest)
		{
			filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
		else if (m_MinFilter == MinFilter::Nearest && m_MagFilter == MagFilter::Nearest && m_MipFilter == MipFilter::Linear)
		{
			filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		}
		else if (m_MinFilter == MinFilter::Nearest && m_MagFilter == MagFilter::Linear && m_MipFilter == MipFilter::Nearest)
		{
			filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		}
		else if (m_MinFilter == MinFilter::Nearest && m_MagFilter == MagFilter::Linear && m_MipFilter == MipFilter::Linear)
		{
			filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		}
		else if (m_MinFilter == MinFilter::Linear && m_MagFilter == MagFilter::Nearest && m_MipFilter == MipFilter::Nearest)
		{
			filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		}
		else if (m_MinFilter == MinFilter::Linear && m_MagFilter == MagFilter::Nearest && m_MipFilter == MipFilter::Linear)
		{
			filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		}
		else if (m_MinFilter == MinFilter::Linear && m_MagFilter == MagFilter::Linear && m_MipFilter == MipFilter::Nearest)
		{
			filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		}
		else if (m_MinFilter == MinFilter::Linear && m_MagFilter == MagFilter::Linear && m_MipFilter == MipFilter::Linear)
		{
			filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}
		else
		{
			Log::CoreError("Invalid texture filter modes");
		}

		if (m_CompareMode != CompareMode::None)
		{
			*(reinterpret_cast<int*>(&filter)) += static_cast<int>(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT);
		}

		return filter;
	}

	D3D11_TEXTURE_ADDRESS_MODE D3D11SamplerState::TranslateWrapMode(const WrapMode wrapMode) const
	{
		switch (wrapMode)
		{
		case WrapMode::Repeat:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case WrapMode::Clamp:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		case WrapMode::Mirror:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		case WrapMode::Border:
			return D3D11_TEXTURE_ADDRESS_BORDER;
		default:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		}
	}

	D3D11_COMPARISON_FUNC D3D11SamplerState::TranslateComparisonFunction(CompareFunc compareFunc) const
	{
		switch (compareFunc)
		{
		case CompareFunc::Never:
			return D3D11_COMPARISON_NEVER;
		case CompareFunc::Less:
			return D3D11_COMPARISON_LESS;
		case CompareFunc::Equal:
			return D3D11_COMPARISON_EQUAL;
		case CompareFunc::LessOrEqual:
			return D3D11_COMPARISON_LESS_EQUAL;
		case CompareFunc::Greater:
			return D3D11_COMPARISON_GREATER;
		case CompareFunc::NotEqual:
			return D3D11_COMPARISON_NOT_EQUAL;
		case CompareFunc::GreaterOrEqual:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case CompareFunc::Always:
			return D3D11_COMPARISON_ALWAYS;
		default:
			return D3D11_COMPARISON_ALWAYS;
		}
	}

	void D3D11SamplerState::Bind(const uint32_t slot, const ShaderType shaderType)
	{
		if (m_IsDirty || m_SamplerState == nullptr)
		{
			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = TranslateFilter();
			samplerDesc.AddressU = TranslateWrapMode(m_WrapModeU);
			samplerDesc.AddressV = TranslateWrapMode(m_WrapModeV);
			samplerDesc.AddressW = TranslateWrapMode(m_WrapModeW);
			samplerDesc.MaxAnisotropy = m_AnisotropicFiltering;
			samplerDesc.ComparisonFunc = TranslateComparisonFunction(m_CompareFunc);
			samplerDesc.BorderColor[0] = m_BorderColor.r;
			samplerDesc.BorderColor[1] = m_BorderColor.g;
			samplerDesc.BorderColor[2] = m_BorderColor.b;
			samplerDesc.BorderColor[3] = m_BorderColor.a;
			samplerDesc.MinLOD = m_MinLOD;
			samplerDesc.MaxLOD = m_MaxLOD;
			samplerDesc.MipLODBias = m_LODBias;

			if (m_SamplerState)
				m_SamplerState->Release();

			const auto res = device->CreateSamplerState(&samplerDesc, &m_SamplerState);
			LEV_CORE_ASSERT(SUCCEEDED(res), "Unable to create SamplerState")

			m_IsDirty = false;
		}

		ID3D11SamplerState* pSamplers[] = { m_SamplerState };

		if (shaderType & ShaderType::Vertex)
			context->VSSetSamplers(slot, 1, pSamplers);
		if (shaderType & ShaderType::Pixel)
			context->PSSetSamplers(slot, 1, pSamplers);
		if (shaderType & ShaderType::Geometry)
			context->GSSetSamplers(slot, 1, pSamplers);
		if (shaderType & ShaderType::Compute)
			context->CSSetSamplers(slot, 1, pSamplers);
	}

	void D3D11SamplerState::Unbind(const uint32_t slot, const ShaderType shaderType)
	{
		ID3D11SamplerState* pSamplers[] = { nullptr };

		if (shaderType & ShaderType::Vertex)
			context->VSSetSamplers(slot, 1, pSamplers);
		if (shaderType & ShaderType::Pixel)
			context->PSSetSamplers(slot, 1, pSamplers);
		if (shaderType & ShaderType::Geometry)
			context->GSSetSamplers(slot, 1, pSamplers);
		if (shaderType & ShaderType::Compute)
			context->CSSetSamplers(slot, 1, pSamplers);
	}
}
