#pragma once

#include "Kernel/Core.h"
#include "Renderer/Pipeline/Viewport.h"

namespace LevEngine
{
    class RenderTechnique;
    struct RenderParams;
    class RenderDebugEvent;
    class Material;
    class Shader;
    struct MaterialShaderVariant;

    class LEV_API RenderPass
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

        // A material can bring its own shader (see MaterialCustom). These two keep the geometry
        // passes on a single code path: ask the material which shader the draw needs, falling
        // back to the pass' own, and swap the bound one only when it actually changes.
        [[nodiscard]] static Ref<Shader> SelectShader(const Material& material, const MaterialShaderVariant& variant,
                                                      const Ref<Shader>& passShader);
        static void BindShader(const Ref<Shader>& shader, Ref<Shader>& boundShader);

    private:
        bool m_Enabled = true;
    };
}
