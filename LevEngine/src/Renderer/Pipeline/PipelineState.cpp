#include "levpch.h"
#include "PipelineState.h"

#include "BlendState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "RenderTarget.h"
#include "Debugging/Profiler.h"
#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
PipelineState::PipelineState()
{
    LEV_PROFILE_FUNCTION();

    m_BlendState = BlendState::Create();
    m_DepthStencilState = DepthStencilState::Create();
    m_RasterizerState = RasterizerState::Create();
}

Ref<Shader> PipelineState::GetShader(const ShaderType type) const
{
    LEV_PROFILE_FUNCTION();

    const auto it = m_Shaders.find(type);
    return it != m_Shaders.end() ? it->second : nullptr;
}

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
