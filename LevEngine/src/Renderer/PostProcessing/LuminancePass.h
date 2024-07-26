#pragma once

#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
    class PipelineState;
    class RenderTarget;
    class Texture;

    class LuminancePass final : public RenderPass
    {
    public:
        explicit LuminancePass(const Ref<Texture>& colorTexture);

        [[nodiscard]] Ref<Texture> GetLuminanceMap() const;

        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<RenderTarget> m_RenderTarget;
        Ref<PipelineState> m_Pipeline;
        Ref<Texture> m_ColorTexture;
    };
}
