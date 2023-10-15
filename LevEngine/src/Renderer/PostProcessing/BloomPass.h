#pragma once

#include "DataTypes/Array.h"
#include "Renderer/RenderPass.h"

namespace LevEngine
{
    class PipelineState;
    class RenderTarget;
    class Texture;

    class BloomPass final : public RenderPass
    {
    public:
        explicit BloomPass(const Ref<Texture>& colorTexture);

        void SetLuminanceMap(const Ref<Texture>& map);
        [[nodiscard]] const Ref<Texture>& GetBloomMap() const;
    
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

        void SetViewport(Viewport viewport) override;

    private:
        Ref<RenderTarget> m_RenderTarget;
        Ref<PipelineState> m_Pipeline;
        Ref<Texture> m_ColorTexture;
        Ref<Texture> m_LuminanceMap;

        Ref<Texture> m_BloomMap;
        Ref<Texture> m_BlurMap;
        Array<Ref<Texture>, 3> m_DownScaleMaps;
    };
}
