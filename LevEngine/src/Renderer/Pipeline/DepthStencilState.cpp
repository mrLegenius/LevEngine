#include "levpch.h"
#include "DepthStencilState.h"

#include "Kernel/Application.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
	DepthMode DepthMode::DisableDepthWrites = DepthMode{ true, DepthWrite::Disable };
	DepthMode DepthMode::DisableDepthTesting = DepthMode{ false };
	
	Ref<DepthStencilState> DepthStencilState::Create()
	{
		return App::RenderDevice().CreateDepthStencilState();
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
