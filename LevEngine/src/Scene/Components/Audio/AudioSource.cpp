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

    AudioSourceComponent::~AudioSourceComponent()
    {
        if (m_EventInstance != nullptr)
        {
            m_Fmod->ReleaseOneEvent(m_EventInstance);
            m_EventInstance = nullptr;
        }
    }

    void AudioSourceComponent::Init()
    {
        // TODO: Replace with AudioSourceInitSystem that is called once on play start when InitSystems will be implemented.

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
        if (m_EventInstance == nullptr)
        {
            if (audioBank == nullptr)
            {
                Log::Warning("Can not load event. No bank assigned to audio source.");
                return;
            }

            String bankPath = ToString(audioBank->GetPath());

            if (m_Fmod->GetBankLoadingStatus(bankPath) !=
                FMOD_STUDIO_LOADING_STATE::FMOD_STUDIO_LOADING_STATE_LOADED)
            {
                m_Fmod->LoadBank(bankPath, FMOD_STUDIO_LOAD_BANK_NORMAL);
                //m_Fmod->LoadBank("D:/projects/ITMO/LevEngine/Sandbox/resources/Audio/Desktop/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);

                int index = bankPath.find('.');
                String bankFilePath = bankPath.left(index);
                String bankStringsPath = bankFilePath + ".strings.bank";
                m_Fmod->LoadBank(bankStringsPath, FMOD_STUDIO_LOAD_BANK_NORMAL);
            }
            
            auto entity = SceneManager::GetActiveScene()->GetEntityByComponent(this);
            m_EventInstance = m_Fmod->CreateSound(String("event:/") + eventName, entity, true);
        }
        else
        {
            m_Fmod->PlaySound(m_EventInstance);
        }
    }

    void AudioSourceComponent::SetPaused(bool isPaused)
    {
        if (m_EventInstance == nullptr)
        {
            Log::Warning("Can not pause sound. No sound loaded.");
            return; 
        }
        
        m_Fmod->SetPause(m_EventInstance, isPaused);
    }

    void AudioSourceComponent::Stop()
    {
        if (m_EventInstance == nullptr)
        {
            Log::Warning("Can not stop sound. No sound loaded.");
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

    class AudioSourceSerializer final : public ComponentSerializer<AudioSourceComponent, AudioSourceSerializer>
    {
    protected:
        const char* GetKey() override { return "Audio Source"; } 

        void SerializeData(YAML::Emitter& out, const AudioSourceComponent& component) override
        {
            SerializeAsset(out, "AudioBank", component.audioBank);
            out << YAML::Key << "AudioEventName" << YAML::Value << component.eventName.data();
            out << YAML::Key << "PlayOnInit" << YAML::Value << component.playOnInit;
        }
        void DeserializeData(YAML::Node& node, AudioSourceComponent& component) override
        {
            component.audioBank = DeserializeAsset<AudioBankAsset>(node["AudioBank"]);
            component.eventName = node["AudioEventName"].as<eastl::string>();
            component.playOnInit = node["PlayOnInit"].as<bool>();
            component.ResetInit();
        }
    };
}
