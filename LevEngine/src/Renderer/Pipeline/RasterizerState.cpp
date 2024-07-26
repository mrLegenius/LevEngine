#include "levpch.h"
#include "RasterizerState.h"

#include "Platform/D3D11/D3D11RasterizerState.h"
#include "Renderer/RenderSettings.h"

namespace LevEngine
{
	RasterizerState::RasterizerState()
	{
		m_Viewports.resize(8, Viewport());
		m_ScissorRects.resize(8, Rect());
	}

	Ref<RasterizerState> RasterizerState::Create()
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_THROW("None for API was chosen")
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11RasterizerState>();
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}

	FillMode RasterizerState::GetFrontFaceFillMode() const
	{
		return m_FrontFaceFillMode;
	}

	void RasterizerState::SetBackFaceFillMode(const FillMode fillMode)
	{
		m_BackFaceFillMode = fillMode;
	}

	FillMode RasterizerState::GetBackFaceFillMode() const
	{
		return m_BackFaceFillMode;
	}

	void RasterizerState::SetCullMode(const CullMode cullMode)
	{
		m_CullMode = cullMode;
		m_StateDirty = true;
	}

	CullMode RasterizerState::GetCullMode() const
	{
		return m_CullMode;
	}

	void RasterizerState::SetFrontFacing(const FrontFace frontFace)
	{
		m_FrontFace = frontFace;
		m_StateDirty = true;
	}

	void RasterizerState::SetDepthClipEnabled(const bool depthClipEnabled)
	{
		m_DepthClipEnabled = depthClipEnabled;
		m_StateDirty = true;
	}

	void RasterizerState::SetViewport(const Viewport& viewport)
	{
		m_Viewports[0] = viewport;
		m_ViewportsDirty = true;
	}

	void RasterizerState::SetViewports(const Vector<Viewport>& viewports)
	{
		m_Viewports = viewports;
		m_ViewportsDirty = true;
	}

	const Vector<Viewport>& RasterizerState::GetViewports() const
	{
		return m_Viewports;
	}

	void RasterizerState::SetScissorEnabled(const bool scissorEnable)
	{
		m_ScissorEnabled = scissorEnable;
		m_StateDirty = true;
	}

	bool RasterizerState::GetScissorEnabled() const
	{
		return m_ScissorEnabled;
	}

	void RasterizerState::SetScissorRect(const Rect& rect)
	{
		m_ScissorRects[0] = rect;
		m_ScissorRectsDirty = true;
	}

	void RasterizerState::SetScissorRects(const Vector<Rect>& rects)
	{
		m_ScissorRects = rects;
		m_ScissorRectsDirty = true;
	}

	const Vector<Rect>& RasterizerState::GetScissorRects() const
	{
		return m_ScissorRects;
	}

	void RasterizerState::SetMultisampleEnabled(const bool multisampleEnabled)
	{
		m_MultisampleEnabled = multisampleEnabled;
		m_StateDirty = true;
	}

	bool RasterizerState::GetMultisampleEnabled() const
	{
		return m_MultisampleEnabled;
	}

	void RasterizerState::SetAntialiasedLineEnable(const bool antialiasedLineEnabled)
	{
		m_AntialiasedLineEnabled = antialiasedLineEnabled;
		m_StateDirty = true;
	}

	bool RasterizerState::GetAntialiasedLineEnable() const
	{
		return m_AntialiasedLineEnabled;
	}

	void RasterizerState::SetForcedSampleCount(const uint8_t sampleCount)
	{
		m_ForcedSampleCount = sampleCount;
		m_StateDirty = true;
	}

	uint8_t RasterizerState::GetForcedSampleCount() const
	{
		return m_ForcedSampleCount;
	}

	void RasterizerState::SetConservativeRasterizationEnabled(const bool conservativeRasterizationEnabled)
	{
		m_ConservativeRasterization = conservativeRasterizationEnabled;
	}

	bool RasterizerState::GetConservativeRasterizationEnabled() const
	{
		// Currently, this implementation always returns false
		// because conservative rasterization is supported since DirectX 11.3 and 12.
		return false;
	}
}
