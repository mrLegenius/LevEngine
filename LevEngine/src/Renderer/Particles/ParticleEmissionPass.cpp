#include "levpch.h"
#include "ParticleEmissionPass.h"

#include "ParticleAssets.h"
#include "ParticlesTextureArray.h"
#include "ParticlesUtils.h"
#include "Assets/TextureAsset.h"
#include "DataTypes/Array.h"
#include "Kernel/Time/Time.h"
#include "Math/Random.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Dispatch/DispatchCommand.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Pipeline/StructuredBuffer.h"
#include "Renderer/Pipeline/Texture.h"
#include "Scene/Components/Emitter/EmitterComponent.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    ParticleEmissionPass::ParticleEmissionPass(const Ref<StructuredBuffer>& particlesBuffer, const Ref<StructuredBuffer>& deadBuffer, const Ref<ParticlesTextureArray>& particlesTextures)
        : m_ParticlesBuffer(particlesBuffer)
          , m_DeadBuffer(deadBuffer)
          , m_ComputeData(ConstantBuffer::Create(sizeof Handler, 1))
          , m_EmitterData(ConstantBuffer::Create(sizeof Emitter, 2))
          , m_RandomData(ConstantBuffer::Create(sizeof RandomGPUData, 3))
          , m_ParticlesTextures(particlesTextures)
    {
    }

    ParticleEmissionPass::~ParticleEmissionPass() = default;

    String ParticleEmissionPass::PassName() { return "Particle Emitter"; }

    bool ParticleEmissionPass::Begin(entt::registry& registry, RenderParams& params)
    {
        const float deltaTime = Time::GetScaledDeltaTime().GetSeconds();
        
        int groupSizeX = 0;
        int groupSizeY = 0;
        ParticlesUtils::GetGroupSize(RenderSettings::MaxParticles, groupSizeX, groupSizeY);
        
        ParticleShaders::Emission()->Bind();
        
        const Handler handler{groupSizeY, RenderSettings::MaxParticles, deltaTime};
        m_ComputeData->SetData(&handler);
        m_ComputeData->Bind(ShaderType::Compute);

        m_ParticlesBuffer->Bind(0, ShaderType::Compute, true);
        m_DeadBuffer->Bind(1, ShaderType::Compute, true);
        
        m_ParticlesTextures->TextureSlots[0] = ParticleTextures::Default();
        m_ParticlesTextures->TextureSlotIndex = 1;
        
        return RenderPass::Begin(registry, params);
    }

    void ParticleEmissionPass::Process(entt::registry& registry, RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();
        
        const float deltaTime = Time::GetScaledDeltaTime().GetSeconds();
        
        const auto group = registry.view<Transform, EmitterComponent>();
        for (const auto entity : group)
        {
            auto [transform, emitter] = group.get<Transform, EmitterComponent>(entity);

            emitter.Timer += deltaTime;
            
            if (Math::IsZero(emitter.Rate)) continue;

            const auto interval = 1 / emitter.Rate;
            uint32_t particlesToEmit = 0;
            while (emitter.Timer >= interval)
            {
                particlesToEmit++;
                emitter.Timer -= interval;
            }

            if (particlesToEmit <= 0) continue;
            
            const auto texture = emitter.Texture ? emitter.Texture->GetTexture() : nullptr;
            const int textureIndex = GetTextureIndex(texture);

            auto emitterData = GetEmitterData(emitter, transform, textureIndex);
            m_EmitterData->SetData(&emitterData);
            m_EmitterData->Bind(ShaderType::Compute);

            RandomGPUData randomData{Random::Int(0, std::numeric_limits<int>::max())};
            m_RandomData->SetData(&randomData);
            m_RandomData->Bind(ShaderType::Compute);

            m_DeadBuffer->BindCounter(4, ShaderType::Compute);
            
            DispatchCommand::Dispatch(particlesToEmit, 1, 1);
        }
    }

    void ParticleEmissionPass::End(entt::registry& registry, RenderParams& params)
    {
        m_ParticlesBuffer->Unbind(0, ShaderType::Compute, true);
        m_DeadBuffer->Unbind(1, ShaderType::Compute, true);
        m_DeadBuffer->UnbindCounter(4, ShaderType::Compute);
    }

    Emitter ParticleEmissionPass::GetEmitterData(EmitterComponent emitter, Transform transform, uint32_t textureIndex)
    {
        LEV_PROFILE_FUNCTION();

        RandomColor color{emitter.Birth.StartColor, emitter.Birth.StartColorB, emitter.Birth.RandomStartColor};
        RandomVector3 velocity{static_cast<Vector4>(emitter.Birth.Velocity), emitter.Birth.VelocityB, emitter.Birth.RandomVelocity};
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

    int ParticleEmissionPass::GetTextureIndex(const Ref<Texture>& texture) const
    {
        if (!texture) return 0;

        int textureIndex = -1;
        for (uint32_t i = 0; i < m_ParticlesTextures->TextureSlotIndex; i++)
        {
            if (m_ParticlesTextures->TextureSlots[i]->GetPath() != texture->GetPath()) continue;
            
            textureIndex = i;
            break;
        }

        if (textureIndex == -1)
        {
            if (m_ParticlesTextures->TextureSlotIndex >= ParticlesTextureArray::MaxTextureSlots)
            {
                textureIndex = 0;
            }
            else
            {
                textureIndex = static_cast<int>(m_ParticlesTextures->TextureSlotIndex);
                m_ParticlesTextures->TextureSlots[m_ParticlesTextures->TextureSlotIndex] = texture;
                m_ParticlesTextures->TextureSlotIndex++;
            }
        }

        return textureIndex;
    }
}
