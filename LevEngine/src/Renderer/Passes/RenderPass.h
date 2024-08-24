#pragma once
#include "Renderer/Pipeline/Viewport.h"

namespace LevEngine
{
    class RenderTechnique;
    struct RenderParams;
    class RenderDebugEvent;

    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;

        void SetEnabled(const bool enabled) { m_Enabled = enabled; }
        [[nodiscard]] bool IsEnabled() const { return m_Enabled; }

        friend RenderTechnique;

        bool Execute(entt::registry& registry, RenderParams& params);

        virtual void SetViewport(Viewport viewport) { }

    protected:
        virtual String PassName() { return {}; }

        virtual bool Begin(entt::registry& registry, RenderParams& params) { return true; }
        virtual void Process(entt::registry& registry, RenderParams& params) = 0;

        virtual void End(entt::registry& registry, RenderParams& params) { }

    private:
        bool m_Enabled = true;
    };
}
