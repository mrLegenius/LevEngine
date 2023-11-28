#pragma once
#include "Math/Color.h"
#include "Math/Matrix.h"
#include "Renderer/RenderPass.h"

namespace LevEngine
{
    class ParticleEmitterPass;
    class BitonicSort;
    class ConstantBuffer;
    class StructuredBuffer;
    class Texture;
    class PipelineState;

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

        struct ParticleCameraData
        {
            Matrix View;
            Matrix Projection;
            alignas(16) Vector3 Position;
        };
    
    public:
        ParticlePass(const Ref<PipelineState>& pipelineState, const Ref<Texture>& depthTexture,
                     const Ref<Texture>& normalTexture);
        
        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<StructuredBuffer> m_ParticlesBuffer;
        Ref<StructuredBuffer> m_DeadBuffer;
        Ref<StructuredBuffer> m_SortedBuffer;

        Ref<PipelineState> m_PipelineState{};
        Ref<ConstantBuffer> m_CameraData{};
        
        Ref<StructuredBuffer> m_TempBuffer{};
        Ref<BitonicSort> m_BitonicSort{};

        Ref<Texture> m_DepthTexture;
        Ref<Texture> m_NormalTexture;

        Scope<ParticleEmitterPass> m_ParticleEmitterPass;
    };
}
