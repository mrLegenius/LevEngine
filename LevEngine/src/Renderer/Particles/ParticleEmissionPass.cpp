#include "levpch.h"
#include "ParticleEmissionPass.h"

#include "ParticleAssets.h"
#include "ParticleBuffers.h"
#include "ParticlesUtils.h"
#include "Kernel/Time/Time.h"
#include "Math/Random.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Dispatch/DispatchCommand.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Pipeline/StructuredBuffer.h"
#include "Scene/Components/Emitter/EmitterComponent.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    ParticleEmissionPass::ParticleEmissionPass() : 
        m_ComputeData(ConstantBuffer::Create(sizeof Handler, 1))
          , m_EmitterData(ConstantBuffer::Create(sizeof Emitter, 2))
          , m_RandomData(ConstantBuffer::Create(sizeof RandomGPUData, 3))
    {
    }

    ParticleEmissionPass::~ParticleEmissionPass() = default;

    String ParticleEmissionPass::PassName() { return "Particle Emitter"; }

    bool ParticleEmissionPass::Begin(entt::registry& registry, RenderParams& params)
    {
        ParticleShaders::Emission()->Bind();
        
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

            if (Math::IsZero(emitter.Rate)) continue;

            const auto& buffers = emitter.GetBuffers();
            if (!buffers) continue;

            emitter.Timer += deltaTime * emitter.Rate;

            uint32_t particlesToEmit = 0;
            while (emitter.Timer >= 1.0f)
            {
                particlesToEmit++;
                emitter.Timer -= 1.0f;
            }

            if (particlesToEmit <= 0) continue;

            int groupSizeX = 0;
            int groupSizeY = 0;
            ParticlesUtils::GetGroupSize(buffers->GetMaxParticlesCount(), groupSizeX, groupSizeY);

            const Handler handler{groupSizeY, buffers->GetMaxParticlesCount(), deltaTime};
            m_ComputeData->SetData(&handler);
            m_ComputeData->Bind(ShaderType::Compute);

            buffers->GetParticlesBuffer()->Bind(0, ShaderType::Compute, true);
            buffers->GetDeadBuffer()->Bind(1, ShaderType::Compute, true);

            auto emitterData = GetEmitterData(emitter, transform);
            m_EmitterData->SetData(&emitterData);
            m_EmitterData->Bind(ShaderType::Compute);

            RandomGPUData randomData{Random::Int(0, std::numeric_limits<int>::max())};
            m_RandomData->SetData(&randomData);
            m_RandomData->Bind(ShaderType::Compute);

            buffers->GetDeadBuffer()->BindCounter(4, ShaderType::Compute);

            DispatchCommand::Dispatch(particlesToEmit, 1, 1);

            buffers->GetParticlesBuffer()->Unbind(0, ShaderType::Compute, true);
            buffers->GetDeadBuffer()->Unbind(1, ShaderType::Compute, true);
            buffers->GetDeadBuffer()->UnbindCounter(4, ShaderType::Compute);
        }
    }

    Emitter ParticleEmissionPass::GetEmitterData(EmitterComponent emitter, Transform transform)
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
                emitter.Birth.GravityScale,
            },
        };
        return emitterData;
    }
}
