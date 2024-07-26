#include "levpch.h"
#include "DepthStencilState.h"

#include "Platform/D3D11/D3D11DepthStencilState.h"
#include "Renderer/RenderSettings.h"

namespace LevEngine
{
	DepthMode DepthMode::DisableDepthWrites = DepthMode{ true, DepthWrite::Disable };
	DepthMode DepthMode::DisableDepthTesting = DepthMode{ false };
	
	Ref<DepthStencilState> DepthStencilState::Create()
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11DepthStencilState>();
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}

		return nullptr;
	}

	void DepthStencilState::SetDepthMode(const DepthMode& depthMode)
	{
		m_DepthMode = depthMode;
		m_Dirty = true;
	}

	void DepthStencilState::SetStencilMode(const StencilMode& stencilMode)
	{
		m_StencilMode = stencilMode;
		m_Dirty = true;
	}
}
