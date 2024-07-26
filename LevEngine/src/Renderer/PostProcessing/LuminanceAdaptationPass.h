#pragma once
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
    class RenderTarget;
    class PipelineState;
    class Texture;
    
    class LuminanceAdaptationPass final : public RenderPass
    {
    public:
        LuminanceAdaptationPass();
    
        [[nodiscard]] Ref<Texture> GetCurrentLuminance() const { return m_LuminanceMapBuffer[m_CurrentMapInBuffer]; }
        
        void SetLuminanceMap(const Ref<Texture>& texture) { m_LuminanceMap = texture; }
        void SwapCurrentLuminanceMap() { m_CurrentMapInBuffer = !m_CurrentMapInBuffer; }

        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<RenderTarget> m_RenderTarget;
        Ref<PipelineState> m_Pipeline;
        Array<Ref<Texture>, 2> m_LuminanceMapBuffer;
        Ref<Texture> m_LuminanceMap;

        int m_CurrentMapInBuffer{};
    };
}
