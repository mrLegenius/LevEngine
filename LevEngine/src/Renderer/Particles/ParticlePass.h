#pragma once
#include "Renderer/RenderPass.h"

#include "ParticleEmissionPass.h"
#include "ParticleRenderingPass.h"
#include "ParticleSimulationPass.h"
#include "ParticleSortingPass.h"

namespace LevEngine
{
    struct ParticlesTextureArray;
    class RenderTarget;
    class BitonicSort;
    class ConstantBuffer;
    class StructuredBuffer;
    class Texture;

    class ParticlePass final : public RenderPass
    {
        struct GPUParticleData
        {
            Vector3 Position;
            Vector3 Velocity;

            Color StartColor;
            Color EndColor;
            Color Color;

            float StartSize;
            float EndSize;
            float Size;

            float LifeTime;
            float Age;
            uint32_t TextureIndex;
            float GravityScale;
        };
    
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
        Ref<StructuredBuffer> m_ParticlesBuffer{};
        Ref<StructuredBuffer> m_DeadBuffer{};
        Ref<StructuredBuffer> m_SortedBuffer{};

        Ref<Texture> m_DepthTexture{};
        Ref<Texture> m_NormalTexture{};

        Scope<ParticleEmissionPass> m_EmissionPass{};
        Scope<ParticleSimulationPass> m_SimulationPass{};
        Scope<ParticleSortingPass> m_SortingPass{};
        Scope<ParticleRenderingPass> m_RenderingPass{};

        Ref<ParticlesTextureArray> m_ParticlesTextures{};
    };
}
