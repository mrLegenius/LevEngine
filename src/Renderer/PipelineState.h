#pragma once
#include <map>

#include "BlendState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "RenderTarget.h"

namespace LevEngine
{
class PipelineState
{
public:
    using ShaderMap = std::map<Shader::Type, Ref<Shader>>;

    PipelineState()
    {
        LEV_PROFILE_FUNCTION();

        m_BlendState = BlendState::Create();
        m_DepthStencilState = DepthStencilState::Create();
        m_RasterizerState = RasterizerState::Create();
    }

    void SetShader(const Shader::Type type, const Ref<Shader>& shader)
    {
        m_Shaders[type] = shader;
    }
    [[nodiscard]] Ref<Shader> GetShader(const Shader::Type type) const
    {
        LEV_PROFILE_FUNCTION();

	    const auto it = m_Shaders.find(type);

        return it != m_Shaders.end() ? it->second : nullptr;
    }
    [[nodiscard]] const ShaderMap& GetShaders() const
    {
        return m_Shaders;
    }

    void SetBlendState(const Ref<BlendState>& blendState) { m_BlendState = blendState; }
    [[nodiscard]] const Ref<BlendState>& GetBlendState() { return m_BlendState; }

    void SetRasterizerState(const Ref<RasterizerState>& rasterizerState) { m_RasterizerState = rasterizerState; }
    [[nodiscard]] RasterizerState& GetRasterizerState() { return *m_RasterizerState.get(); }

    void SetDepthStencilState(const Ref<DepthStencilState>& depthStencilState) { m_DepthStencilState = depthStencilState; }
    [[nodiscard]] const Ref<DepthStencilState>& GetDepthStencilState() { return m_DepthStencilState; }

    void SetRenderTarget(const Ref<RenderTarget>& renderTarget) { m_RenderTarget = renderTarget; }
    [[nodiscard]] Ref<RenderTarget> GetRenderTarget() const { return m_RenderTarget; }

    void Bind() const;
    void Unbind() const;
private:
    ShaderMap m_Shaders;

    Ref<BlendState> m_BlendState;
    Ref<RasterizerState> m_RasterizerState;
    Ref<DepthStencilState> m_DepthStencilState;
    Ref<RenderTarget> m_RenderTarget;
};
}