#pragma once

#include "Kernel/Core.h"
#include "Renderer/Passes/RenderPass.h"

#include "AtmosphereConstants.h"
#include "EnvironmentPrecomputePass.h"
#include "EnvironmentRenderPass.h"

namespace LevEngine
{
    class RenderTarget;

    class LEV_API EnvironmentPass final : public RenderPass
    {
    public:
        explicit EnvironmentPass(const Ref<RenderTarget>& renderTarget);

        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void SetViewport(Viewport viewport) override;

        //<--- Shared with PlanetAtmospherePass: one atmosphere, collected once a frame ---<<
        [[nodiscard]] const Ref<AtmosphereConstants>& GetAtmosphere() const { return m_Atmosphere; }

    private:
        Ref<AtmosphereConstants> m_Atmosphere;
        Scope<EnvironmentPrecomputePass> m_PrecomputePass;
        Scope<EnvironmentRenderPass> m_RenderPass;
    };
}
