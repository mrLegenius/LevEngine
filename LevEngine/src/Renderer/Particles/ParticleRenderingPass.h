#pragma once
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
    class ParticleBuffers;
    struct ParticlesTextureArray;
    class RenderTarget;
    class ConstantBuffer;
    class PipelineState;

    struct ParticleCameraData
    {
        Matrix View;
        Matrix Projection;
        alignas(16) Vector3 Position;
    };
    
    class ParticleRenderingPass final : public RenderPass
    {
    public:
        ParticleRenderingPass(const Ref<RenderTarget>& renderTarget);
        
        ~ParticleRenderingPass() override;
        
        void SetViewport(Viewport viewport) override;
        
    protected:
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<PipelineState> m_PipelineState{};
    };
}
