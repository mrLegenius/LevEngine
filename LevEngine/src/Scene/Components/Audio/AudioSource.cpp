#include "levpch.h"
#include "AudioSource.h"

#include "Audio/AudioPlayer.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Scene/SceneManager.h"
#include "Assets/AudioBankAsset.h"

namespace LevEngine
{
    AudioSourceComponent::AudioSourceComponent()
    {
        Player = CreateRef<AudioPlayer>();
    }

    void AudioSourceComponent::OnConstruct(entt::registry& registry, entt::entity entity)
    {
        auto entityWrapped = Entity(entt::handle{ registry, entity });
        AudioSourceComponent& component = entityWrapped.GetComponent<AudioSourceComponent>();
        component.Init(entityWrapped);
    }

    void AudioSourceComponent::Init(Entity entity)
    {
        if (IsInitialized()) return;

        m_IsInited = true;

        Player->Play(entity);
    }

    bool AudioSourceComponent::IsInitialized() const
    {
        return m_IsInited;
    }

    void AudioSourceComponent::ResetInit()
    {
        m_IsInited = false;
    }

    class AudioSourceSerializer final : public ComponentSerializer<AudioSourceComponent, AudioSourceSerializer>
    {
    protected:
        const char* GetKey() override { return "Audio Source"; } 

        void SerializeData(YAML::Emitter& out, const AudioSourceComponent& component) override
        {
            SerializeAsset(out, "AudioBank", component.Player->GetAudioBankAsset());
            out << YAML::Key << "AudioEventName" << YAML::Value << component.Player->GetEventName().data();
            out << YAML::Key << "PlayOnInit" << YAML::Value << component.Player->GetPlayOnInit();
            out << YAML::Key << "IsOneShot" << YAML::Value << component.Player->GetIsOneShot();
        }
        void DeserializeData(const YAML::Node& node, AudioSourceComponent& component) override
        {
            if (const auto audioBankNode = node["AudioBank"])
            {
                component.Player->SetAudioBankAsset(DeserializeAsset<AudioBankAsset>(audioBankNode));
            }

            if (const auto audioEventNameNode = node["AudioEventName"])
            {
                component.Player->SetEventName(audioEventNameNode.as<String>());
            }

            if (const auto audioPlayerNode = node["PlayOnInit"])
            {
                component.Player->SetPlayOnInit(audioPlayerNode.as<bool>());
            }

            if (const auto isOneShotNode = node["IsOneShot"])
            {
                component.Player->SetIsOneShot(isOneShotNode.as<bool>());
            }

            component.ResetInit();
        }
    };
}
