#pragma once

namespace LevEngine
{
    enum class ShaderType;
    class RenderTarget;
    class DepthStencilState;
    class RasterizerState;
    class BlendState;
    class Shader;

    class PipelineState
    {
    public:
        using ShaderMap = Map<ShaderType, Ref<Shader>>;

        PipelineState();

        void SetShader(const ShaderType type, const Ref<Shader>& shader) { m_Shaders[type] = shader; }
        [[nodiscard]] Ref<Shader> GetShader(ShaderType type) const;
        [[nodiscard]] const ShaderMap& GetShaders() const { return m_Shaders; }

        void SetBlendState(const Ref<BlendState>& blendState) { m_BlendState = blendState; }
        [[nodiscard]] const Ref<BlendState>& GetBlendState() const { return m_BlendState; }

        void SetRasterizerState(const Ref<RasterizerState>& rasterizerState) { m_RasterizerState = rasterizerState; }
        [[nodiscard]] RasterizerState& GetRasterizerState() const { return *m_RasterizerState.get(); }

        void SetDepthStencilState(const Ref<DepthStencilState>& depthStencilState)
        {
            m_DepthStencilState = depthStencilState;
        }

        [[nodiscard]] Ref<DepthStencilState>& GetDepthStencilState() { return m_DepthStencilState; }

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
