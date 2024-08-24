#pragma once

namespace LevEngine
{
    class RenderTarget;
    class Texture;
    class RasterizerState;

    class CascadeShadowMap
    {
    public:
        CascadeShadowMap(uint32_t width, uint32_t height);

        void Bind(uint32_t slot = 0) const;
        void SetRenderTarget() const;
        void ResetRenderTarget() const;

    private:
        Ref<RasterizerState> m_RastState{};
        Ref<Texture> m_Texture{};
        Ref<RenderTarget> m_RenderTarget{};
    };
}
