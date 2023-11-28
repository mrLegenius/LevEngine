#pragma once
#include "Math/Color.h"
#include "Renderer/RenderPass.h"

namespace LevEngine
{
    struct EmitterComponent;
    struct Transform;
    class ConstantBuffer;
    class StructuredBuffer;

    struct alignas(16) Handler
    {
        int GroupDim;
        uint32_t MaxParticles;
        float DeltaTime;
    };
    
    struct RandomFloat
    {
        float From;
        float To;
        int Randomize = false;

        float pad;
    };

    struct RandomVector3
    {
        alignas(16) Vector4 From;
        Vector3 To;
        int Randomize = false;
    };

    struct RandomColor
    {
        alignas(16) Color From;
        alignas(16) Color To;
        int Randomize = false;

        float pad[3];
    };

    struct Emitter
    {
        struct BirthParams
        {
            alignas(16) RandomVector3 Velocity;
            alignas(16) RandomVector3 Position;

            alignas(16) RandomColor StartColor;
            alignas(16) Color EndColor;

            alignas(16) RandomFloat StartSize;
            float EndSize;
            alignas(16) RandomFloat LifeTime;

            //<--- 16 byte ---<<
            uint32_t TextureIndex;
            float GravityScale;
        };

        BirthParams Birth;
    };

    struct RandomGPUData
    {
        alignas(16) int RandomSeed;
    };
    
    class ParticleEmitterPass final : public RenderPass
    {
    public:
        ParticleEmitterPass(const Ref<StructuredBuffer>& particlesBuffer, const Ref<StructuredBuffer>& deadBuffer);
        ~ParticleEmitterPass() override;
        
    protected:
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<StructuredBuffer> m_ParticlesBuffer{};
        Ref<StructuredBuffer> m_DeadBuffer;
        
        Ref<ConstantBuffer> m_ComputeData{};
        Ref<ConstantBuffer> m_EmitterData{};
        Ref<ConstantBuffer> m_RandomData{};
        
        static Emitter GetEmitterData(EmitterComponent emitter, Transform transform, uint32_t textureIndex);
    };
}
