#include "levpch.h"
#include "ParticleEmitterPass.h"

#include "ParticlesUtils.h"
#include "Assets/TextureAsset.h"
#include "DataTypes/Array.h"
#include "Kernel/Time/Time.h"
#include "Math/Random.h"
#include "Platform/D3D11/D3D11RendererContext.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Shader.h"
#include "Renderer/StructuredBuffer.h"
#include "Renderer/Texture.h"
#include "Scene/Components/Components.h"
#include "Scene/Components/Emitter/EmitterComponent.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    //TODO: Get Rid of it
    extern ID3D11DeviceContext* context;

    ParticleEmitterPass::ParticleEmitterPass(const Ref<StructuredBuffer>& particlesBuffer, const Ref<StructuredBuffer>& deadBuffer)
        : m_ParticlesBuffer(particlesBuffer)
          , m_DeadBuffer(deadBuffer)
          , m_ComputeData(ConstantBuffer::Create(sizeof Handler, 1))
          , m_EmitterData(ConstantBuffer::Create(sizeof Emitter, 2))
          , m_RandomData(ConstantBuffer::Create(sizeof RandomGPUData, 3))
    { }

    ParticleEmitterPass::~ParticleEmitterPass() = default;

    String ParticleEmitterPass::PassName() { return "Particle Emitter"; }

    void ParticleEmitterPass::Process(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        constexpr uint32_t MaxTextureSlots = 32;
        Array<Ref<Texture>, MaxTextureSlots> textureSlots{};
        textureSlots[0] = TextureAssets::Particle(); //<--- default particle ---<<
        uint32_t textureSlotIndex = 1;

        const float deltaTime = Time::GetScaledDeltaTime().GetSeconds();
        
        int groupSizeX = 0;
        int groupSizeY = 0;
        ParticlesUtils::GetGroupSize(RenderSettings::MaxParticles, groupSizeX, groupSizeY);

        const Handler handler{groupSizeY, RenderSettings::MaxParticles, deltaTime};
        m_ComputeData->SetData(&handler);
        m_ComputeData->Bind(ShaderType::Compute);

        m_ParticlesBuffer->Bind(0, ShaderType::Compute, true, -1);
        m_DeadBuffer->Bind(1, ShaderType::Compute, true, -1);

        const auto group = registry.view<Transform, EmitterComponent, IDComponent>();
        for (const auto entity : group)
        {
            auto [transform, emitter, id] = group.get<Transform, EmitterComponent, IDComponent>(entity);

            int textureIndex = -1;

            const auto texture = emitter.Texture ? emitter.Texture->GetTexture() : nullptr;
            for (uint32_t i = 0; i < textureSlotIndex; i++)
            {
                if (!texture)
                {
                    textureIndex = 0;
                    break;
                }

                if (textureSlots[i]->GetPath() == texture->GetPath())
                {
                    textureIndex = i;
                    break;
                }
            }

            if (textureIndex == -1)
            {
                if (textureSlotIndex >= MaxTextureSlots)
                {
                    textureIndex = 0;
                }
                else
                {
                    textureIndex = textureSlotIndex;
                    textureSlots[textureSlotIndex] = texture;
                    textureSlotIndex++;
                }
            }

            auto emitterData = GetEmitterData(emitter, transform, textureIndex);
            m_EmitterData->SetData(&emitterData);
            m_EmitterData->Bind(ShaderType::Compute);

            RandomGPUData randomData{Random::Int(0, std::numeric_limits<int>::max())};
            m_RandomData->SetData(&randomData);
            m_RandomData->Bind(ShaderType::Compute);

            //<--- Emit ---<<

            emitter.Timer += deltaTime;
            const auto interval = 1 / emitter.Rate;
            uint32_t particlesToEmit = 0;
            if (emitter.Timer > interval)
            {
                particlesToEmit++;
                emitter.Timer -= interval;
            }

            ShaderAssets::ParticlesEmitter()->Bind();
            const uint32_t deadParticlesCount = m_DeadBuffer->GetCounterValue();
            particlesToEmit = Math::Min(deadParticlesCount, particlesToEmit);
            if (particlesToEmit > 0)
                context->Dispatch(particlesToEmit, 1, 1);
        }
    }

    Emitter ParticleEmitterPass::GetEmitterData(EmitterComponent emitter, Transform transform, uint32_t textureIndex)
    {
        LEV_PROFILE_FUNCTION();

        RandomColor color{emitter.Birth.StartColor, emitter.Birth.StartColorB, emitter.Birth.RandomStartColor};
        RandomVector3 velocity{(Vector4)emitter.Birth.Velocity, emitter.Birth.VelocityB, emitter.Birth.RandomVelocity};
        RandomVector3 position{
            static_cast<Vector4>(transform.GetWorldPosition()) + emitter.Birth.Position,
            transform.GetWorldPosition() + emitter.Birth.PositionB, emitter.Birth.RandomStartPosition
        };
        RandomFloat size{emitter.Birth.StartSize, emitter.Birth.StartSizeB, emitter.Birth.RandomStartSize};
        RandomFloat lifeTime{emitter.Birth.LifeTime, emitter.Birth.LifeTimeB, emitter.Birth.RandomStartLifeTime};

        auto emitterData = Emitter{
            Emitter::BirthParams{
                velocity,
                position,
                color,
                emitter.Birth.EndColor,
                size,
                emitter.Birth.EndSize,
                lifeTime,
                textureIndex,
                emitter.Birth.GravityScale,
            },
        };
        return emitterData;
    }
}
