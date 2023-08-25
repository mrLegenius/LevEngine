#include "levpch.h"
#include "PipelineState.h"

#include "Debugging/Profiler.h"
namespace LevEngine
{
void PipelineState::Bind() const
{
    LEV_PROFILE_FUNCTION();

    if (m_RenderTarget)
	    m_RenderTarget->Bind();

    m_BlendState->Bind();
    m_RasterizerState->Bind();
    m_DepthStencilState->Bind();
    for (auto& [type, shader] : m_Shaders)
    {
        if (shader)
            shader->Bind();
    }
}

void PipelineState::Unbind() const
{
    LEV_PROFILE_FUNCTION();

    if (m_RenderTarget)
	    m_RenderTarget->Unbind();

    m_BlendState->Unbind();
    m_RasterizerState->Unbind();
    m_DepthStencilState->Unbind();

    for (auto& [type, shader] : m_Shaders)
    {
        if (shader)
			shader->Unbind();
    }
}
}