#include "pch.h"
#include "PipelineState.h"

#include "Debugging/Profiler.h"
namespace LevEngine
{
void PipelineState::Bind()
{
    LEV_PROFILE_FUNCTION();

    if (m_RenderTarget)
	    m_RenderTarget->Bind();

    m_BlendState.Bind();
    m_RasterizerState.Bind();
    m_DepthStencilState.Bind();
}

void PipelineState::Unbind()
{
    LEV_PROFILE_FUNCTION();

    if (m_RenderTarget)
	    m_RenderTarget->Unbind();

    m_BlendState.Unbind();
    m_RasterizerState.Unbind();
    m_DepthStencilState.Unbind();
}
}