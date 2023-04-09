#pragma once
#include "D3D11BlendState.h"
#include "D3D11DepthStencilState.h"
#include "D3D11RasterizerState.h"
#include "D3D11RenderTarget.h"

class PipelineState
{
public:
    PipelineState() = default;
    void SetBlendState(const D3D11BlendState& blendState) { m_BlendState = blendState; }
    [[nodiscard]] D3D11BlendState& GetBlendState() { return m_BlendState; }

    void SetRasterizerState(const D3D11RasterizerState& rasterizerState) { m_RasterizerState = rasterizerState; }
    [[nodiscard]] D3D11RasterizerState& GetRasterizerState() { return m_RasterizerState; }

    void SetDepthStencilState(const D3D11DepthStencilState& depthStencilState) { m_DepthStencilState = depthStencilState; }
    [[nodiscard]] D3D11DepthStencilState& GetDepthStencilState() { return m_DepthStencilState; }

    void SetRenderTarget(const Ref<D3D11RenderTarget> renderTarget) { m_RenderTarget = renderTarget; }
    [[nodiscard]] Ref<D3D11RenderTarget> GetRenderTarget() const { return m_RenderTarget; }

    // Bind this pipeline state for rendering.
    void Bind();
    // Unbind render target
    void Unbind();
private:
    D3D11BlendState m_BlendState;
    D3D11RasterizerState m_RasterizerState;
    D3D11DepthStencilState m_DepthStencilState;
    Ref<D3D11RenderTarget> m_RenderTarget;
};
