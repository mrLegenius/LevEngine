#include "levpch.h"
#include "AudioSource.h"

#include "FMOD/LevFmod.h"
#include "Kernel/Application.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Scene/SceneManager.h"

namespace LevEngine
{
    AudioSourceComponent::AudioSourceComponent()
    {
        m_Fmod = Application::Get().GetAudioSubsystem();
        m_EventInstance = nullptr;
    }

    void AudioSourceComponent::OnComponentConstruct(entt::registry& registry, entt::entity entity)
    {
        AudioSourceComponent& component = registry.get<AudioSourceComponent>(entity);
        component.Init();
    }

    void AudioSourceComponent::OnComponentDestroy(entt::registry& registry, entt::entity entity)
    {
        AudioSourceComponent& component = registry.get<AudioSourceComponent>(entity);

        if (component.m_EventInstance != nullptr)
        {
            component.m_Fmod->ReleaseOneEvent(component.m_EventInstance, FMOD_STUDIO_STOP_ALLOWFADEOUT);
            component.m_EventInstance = nullptr;
        }
    }

    void AudioSourceComponent::Init()
    {
        if (IsInitialized())
        {
            return;
        }

        m_IsInited = true;

        if (playOnInit)
        {
            Play();
        }
    }

    bool AudioSourceComponent::IsInitialized() const
    {
        return m_IsInited;
    }

    void AudioSourceComponent::ResetInit()
    {
        m_IsInited = false;
    }

    void AudioSourceComponent::Play()
    {
        if (m_EventInstance == nullptr || isOneShot)
        {
            if (audioBank == nullptr)
            {
                Log::Warning("Can not load event. No bank assigned to audio source.");
                return;
            }

            String bankPath = ToString(audioBank->GetPath());

            if (!IsBankLoaded(bankPath))
            {
                LoadBank(bankPath);
            }

            Entity entity = SceneManager::GetActiveScene()->GetEntityByComponent(this);

            if (!entity)
            {
                return;
            }

            String eventNameWithPrefix = String("event:/") + eventName;

            if (isOneShot)
            {
                m_Fmod->PlayOneShot(eventNameWithPrefix, entity);
                entity.RemoveComponent<AudioSourceComponent>();
                return;
            }
            
            m_EventInstance = m_Fmod->CreateSound(eventNameWithPrefix, entity, true);
        }
        else
        {
            m_Fmod->PlaySound(m_EventInstance);
        }
    }

    bool AudioSourceComponent::IsBankLoaded(String& bankPath)
    {
        return m_Fmod->GetBankLoadingStatus(bankPath) ==
            FMOD_STUDIO_LOADING_STATE::FMOD_STUDIO_LOADING_STATE_LOADED;
    }

    void AudioSourceComponent::LoadBank(String& bankPath)
    {
        m_Fmod->LoadBank(bankPath, FMOD_STUDIO_LOAD_BANK_NORMAL);

        int index = bankPath.find('.');
        String bankFilePath = bankPath.left(index);
        String bankStringsPath = bankFilePath + ".strings.bank";
        m_Fmod->LoadBank(bankStringsPath, FMOD_STUDIO_LOAD_BANK_NORMAL);
    }

    void AudioSourceComponent::SetPaused(bool isPaused)
    {
        if (isOneShot)
        {
            Log::Warning("Can not pause a one shot sound.");
            return;
        }

        if (m_EventInstance == nullptr)
        {
            Log::Warning("Can not pause sound. No sound instance loaded.");
            return; 
        }
        
        m_Fmod->SetPause(m_EventInstance, isPaused);
    }

    void AudioSourceComponent::Stop()
    {
        if (isOneShot)
        {
            Log::Warning("Can not stop a one shot sound.");
            return;
        }

        if (m_EventInstance == nullptr)
        {
            Log::Warning("Can not stop sound. No sound instance loaded.");
            return;
        }

        m_Fmod->StopSound(m_EventInstance);
    }

    bool AudioSourceComponent::GetPlayOnInit() const
    {
        return this->playOnInit;
    }

    void AudioSourceComponent::SetPlayOnInit(bool playOnInit)
    {
        this->playOnInit = playOnInit;
    }

    bool AudioSourceComponent::GetIsOneShot() const
    {
        return this->isOneShot;
    }

    void AudioSourceComponent::SetIsOneShot(bool isOneShot)
    {
        this->isOneShot = isOneShot;
    }

    class AudioSourceSerializer final : public ComponentSerializer<AudioSourceComponent, AudioSourceSerializer>
    {
    protected:
        const char* GetKey() override { return "Audio Source"; } 

        void SerializeData(YAML::Emitter& out, const AudioSourceComponent& component) override
        {
            SerializeAsset(out, "AudioBank", component.audioBank);
            out << YAML::Key << "AudioEventName" << YAML::Value << component.eventName.data();
            out << YAML::Key << "PlayOnInit" << YAML::Value << component.playOnInit;
            out << YAML::Key << "IsOneShot" << YAML::Value << component.isOneShot;
        }
        void DeserializeData(YAML::Node& node, AudioSourceComponent& component) override
        {
            if (node["AudioBank"])
                component.audioBank = DeserializeAsset<AudioBankAsset>(node["AudioBank"]);

            if (node["AudioEventName"])
                component.eventName = node["AudioEventName"].as<eastl::string>();

            if (node["PlayOnInit"])
                component.playOnInit = node["PlayOnInit"].as<bool>();

            if (node["IsOneShot"])
                component.isOneShot = node["IsOneShot"].as<bool>();

            component.ResetInit();
        }
    };
}
