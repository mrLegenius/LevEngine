#pragma once
#include "BlendState.h"
#include "DepthStencilState.h"
#include "D3D11RasterizerState.h"
#include "D3D11RenderTarget.h"
namespace LevEngine
{
class PipelineState
{
public:
    PipelineState()
    {
        m_BlendState = BlendState::Create();
        m_DepthStencilState = DepthStencilState::Create();
    }

    void SetBlendState(const Ref<BlendState>& blendState) { m_BlendState = blendState; }
    [[nodiscard]] const Ref<BlendState>& GetBlendState() { return m_BlendState; }

    void SetRasterizerState(const D3D11RasterizerState& rasterizerState) { m_RasterizerState = rasterizerState; }
    [[nodiscard]] D3D11RasterizerState& GetRasterizerState() { return m_RasterizerState; }

    void SetDepthStencilState(const Ref<DepthStencilState>& depthStencilState) { m_DepthStencilState = depthStencilState; }
    [[nodiscard]] const Ref<DepthStencilState>& GetDepthStencilState() { return m_DepthStencilState; }

    void SetRenderTarget(const Ref<D3D11RenderTarget>& renderTarget) { m_RenderTarget = renderTarget; }
    [[nodiscard]] Ref<D3D11RenderTarget> GetRenderTarget() const { return m_RenderTarget; }

    // Bind this pipeline state for rendering.
    void Bind();
    // Unbind render target
    void Unbind();
private:
    Ref<BlendState> m_BlendState;
    D3D11RasterizerState m_RasterizerState;
    Ref<DepthStencilState> m_DepthStencilState;
    Ref<D3D11RenderTarget> m_RenderTarget;
};
}