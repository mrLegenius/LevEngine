#pragma once
#include "Renderer/RenderPass.h"

namespace LevEngine
{
    struct ParticlesTextureArray;
    class RenderTarget;
    class ConstantBuffer;
    class PipelineState;
    class StructuredBuffer;

    struct ParticleCameraData
    {
        Matrix View;
        Matrix Projection;
        alignas(16) Vector3 Position;
    };
    
    class ParticleRenderingPass final : public RenderPass
    {
    public:
        ParticleRenderingPass(
            const Ref<RenderTarget>& renderTarget,
            const Ref<StructuredBuffer>& particlesBuffer,
            const Ref<StructuredBuffer>& sortedBuffer,
            const Ref<ParticlesTextureArray>& particlesTextures);
        
        ~ParticleRenderingPass() override;
        
        void SetViewport(Viewport viewport) override;
        
    protected:
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<StructuredBuffer> m_ParticlesBuffer{};
        Ref<StructuredBuffer> m_SortedBuffer{};

        Ref<PipelineState> m_PipelineState{};
        Ref<ConstantBuffer> m_CameraData{};
        const Ref<ParticlesTextureArray>& m_ParticlesTextures;
    };
}
