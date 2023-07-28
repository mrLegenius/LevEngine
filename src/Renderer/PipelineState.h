#pragma once
#include "BlendState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "RenderTarget.h"

namespace LevEngine
{
class PipelineState
{
public:
    PipelineState()
    {
        m_BlendState = BlendState::Create();
        m_DepthStencilState = DepthStencilState::Create();
        m_RasterizerState = RasterizerState::Create();
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
    Ref<BlendState> m_BlendState;
    Ref<RasterizerState> m_RasterizerState;
    Ref<DepthStencilState> m_DepthStencilState;
    Ref<RenderTarget> m_RenderTarget;
};
}