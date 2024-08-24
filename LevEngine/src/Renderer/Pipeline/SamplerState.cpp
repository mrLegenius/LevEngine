#include "levpch.h"
#include "SamplerState.h"

#include "Kernel/Application.h"
#include "Math/Math.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
	Ref<SamplerState> SamplerState::Create()
	{
		return App::RenderDevice().CreateSamplerState();
	}

	void SamplerState::SetFilter(const Filter minFilter, const Filter magFilter, const Filter mipFilter)
	{
		m_MinFilter = minFilter;
		m_MagFilter = magFilter;
		m_MipFilter = mipFilter;
		m_IsDirty = true;
	}

	void SamplerState::GetFilter(Filter& minFilter, Filter& magFilter, Filter& mipFilter) const
	{
		minFilter = m_MinFilter;
		magFilter = m_MagFilter;
		mipFilter = m_MipFilter;
	}

	void SamplerState::SetWrapMode(const WrapMode u, const WrapMode v, const WrapMode w)
	{
		m_WrapModeU = u;
		m_WrapModeV = v;
		m_WrapModeW = w;
		m_IsDirty = true;
	}

	void SamplerState::GetWrapMode(WrapMode& u, WrapMode& v, WrapMode& w) const
	{
		u = m_WrapModeU;
		v = m_WrapModeV;
		w = m_WrapModeW;
	}

	void SamplerState::SetCompareMode(const CompareMode compareMode)
	{
		m_CompareMode = compareMode;
		m_IsDirty = true;
	}
	SamplerState::CompareMode SamplerState::GetCompareMode() const
	{
		return m_CompareMode;
	}

	void SamplerState::SetCompareFunction(const CompareFunc compareFunc)
	{
		m_CompareFunc = compareFunc;
		m_IsDirty = true;
	}
	SamplerState::CompareFunc SamplerState::GetCompareFunction() const
	{
		return m_CompareFunc;
	}

	void SamplerState::SetLODBias(const float lodBias)
	{
		m_LODBias = lodBias;
		m_IsDirty = true;
	}
	float SamplerState::GetLODBias() const
	{
		return m_LODBias;
	}

	void SamplerState::SetMinLOD(const float minLOD)
	{
		m_MinLOD = minLOD;
		m_IsDirty = true;
	}
	float SamplerState::GetMinLOD() const
	{
		return m_MinLOD;
	}

	void SamplerState::SetMaxLOD(const float maxLOD)
	{
		m_MaxLOD = maxLOD;
		m_IsDirty = true;
	}
	float SamplerState::GetMaxLOD() const
	{
		return m_MaxLOD;
	}

	void SamplerState::SetBorderColor(const Color& borderColor)
	{
		m_BorderColor = borderColor;
		m_IsDirty = true;
	}
	Color SamplerState::GetBorderColor() const
	{
		return m_BorderColor;
	}

	void SamplerState::EnableAnisotropicFiltering(const bool enabled)
	{
		m_IsAnisotropicFilteringEnabled = enabled;
		m_IsDirty = true;
	}
	bool SamplerState::IsAnisotropicFilteringEnabled() const
	{
		return m_IsAnisotropicFilteringEnabled;
	}

	void SamplerState::SetMaxAnisotropy(const uint8_t maxAnisotropy)
	{
		m_AnisotropicFiltering = Math::Clamp<uint8_t>(maxAnisotropy, 1, 16);
		m_IsDirty = true;
	}
	uint8_t SamplerState::GetMaxAnisotropy() const
	{
		return m_AnisotropicFiltering;
	}
}
