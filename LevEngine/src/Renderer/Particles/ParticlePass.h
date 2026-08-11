#pragma once
#include "Renderer/Passes/RenderPass.h"

#include "ParticleEmissionPass.h"
#include "ParticleRenderingPass.h"
#include "ParticleSimulationPass.h"
#include "ParticleSortingPass.h"

namespace LevEngine
{
    class RenderTarget;
    class ConstantBuffer;
    class Texture;


    class ParticlePass final : public RenderPass
    {
    public:
        ParticlePass(const Ref<RenderTarget>& renderTarget,
                    const Ref<Texture>& depthTexture,
                     const Ref<Texture>& normalTexture);
        
        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;
        void SetViewport(Viewport viewport) override;
        
    private:
        Ref<ConstantBuffer> m_CameraData{};
        
        Ref<Texture> m_DepthTexture{};
        Ref<Texture> m_NormalTexture{};

        Scope<ParticleEmissionPass> m_EmissionPass{};
        Scope<ParticleSimulationPass> m_SimulationPass{};
        Scope<ParticleSortingPass> m_SortingPass{};
        Scope<ParticleRenderingPass> m_RenderingPass{};
    };
}
