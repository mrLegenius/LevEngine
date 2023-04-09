#include "PipelineState.h"

void PipelineState::Bind()
{
    if (m_RenderTarget)
    {
        m_RenderTarget->Bind();
    }

    m_BlendState.Bind();
    m_RasterizerState.Bind();
    m_DepthStencilState.Bind();
}

void PipelineState::Unbind()
{
    if (m_RenderTarget)
	    m_RenderTarget->Unbind();

    m_BlendState.Unbind();
    m_RasterizerState.Unbind();
    m_DepthStencilState.Unbind();
}
