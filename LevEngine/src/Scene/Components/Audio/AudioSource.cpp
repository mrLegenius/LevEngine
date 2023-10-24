#include "levpch.h"
#include "AudioSource.h"

#include "Audio/AudioPlayer.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Scene/SceneManager.h"

namespace LevEngine
{
    AudioSourceComponent::AudioSourceComponent()
    {
        audioPlayer = CreateRef<AudioPlayer>();
    }

    void AudioSourceComponent::OnComponentConstruct(entt::registry& registry, entt::entity entity)
    {
        AudioSourceComponent& component = registry.get<AudioSourceComponent>(entity);
        component.Init();
    }

    void AudioSourceComponent::Init()
    {
        if (IsInitialized())
        {
            return;
        }

        m_IsInited = true;

        Entity entity = SceneManager::GetActiveScene()->GetEntityByComponent(this);
        audioPlayer->Play(entity);
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
            SerializeAsset(out, "AudioBank", component.audioPlayer->GetAudioBankAsset());
            out << YAML::Key << "AudioEventName" << YAML::Value << component.audioPlayer->GetEventName().data();
            out << YAML::Key << "PlayOnInit" << YAML::Value << component.audioPlayer->GetPlayOnInit();
            out << YAML::Key << "IsOneShot" << YAML::Value << component.audioPlayer->GetIsOneShot();
        }
        void DeserializeData(YAML::Node& node, AudioSourceComponent& component) override
        {
            if (node["AudioBank"])
            {
                component.audioPlayer->SetAudioBankAsset(DeserializeAsset<AudioBankAsset>(node["AudioBank"]));
            }

            if (node["AudioEventName"])
            {
                component.audioPlayer->SetEventName(node["AudioEventName"].as<eastl::string>());
            }

            if (node["PlayOnInit"])
            {
                component.audioPlayer->SetPlayOnInit(node["PlayOnInit"].as<bool>());
            }

            if (node["IsOneShot"])
            {
                component.audioPlayer->SetIsOneShot(node["IsOneShot"].as<bool>());
            }

            component.ResetInit();
        }
    };
}
