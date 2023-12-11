#pragma once
#include "RenderPass.h"
#include "PipelineState.h"

namespace LevEngine
{
    class Texture;

    class QuadRenderPass : public RenderPass
    {
    public:
        explicit QuadRenderPass(const Ref<PipelineState>& pipeline, const Ref<Texture>& colorTexture)
            : m_Pipeline(pipeline), m_ColorTexture(colorTexture)
        {
        }

    protected:
        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<PipelineState> m_Pipeline;
        Ref<Texture> m_ColorTexture;
    };
}
