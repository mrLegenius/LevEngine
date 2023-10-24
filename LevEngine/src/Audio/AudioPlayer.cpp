#include "levpch.h"
#include "AudioPlayer.h"

#include "Audio/LevFmod.h"
#include "Assets/AudioBankAsset.h"
#include "Scene/Components/Audio/AudioSource.h"
#include "Scene/SceneManager.h"

namespace LevEngine
{
    AudioPlayer::AudioPlayer()
    {
        m_Fmod = SceneManager::GetActiveScene()->GetAudioSubsystem();
        m_EventHandle = 0;
    }

    AudioPlayer::~AudioPlayer()
    {
        ReleaseSound();
        m_Fmod = nullptr;
    }

    void AudioPlayer::ReleaseSound()
    {
        if (m_EventHandle != 0)
        {
            m_Fmod->ReleaseOneEvent(m_EventHandle);
            m_EventHandle = 0;
        }
    }

    void AudioPlayer::Play(Entity audioSourceEntity)
    {
        if (m_EventHandle == 0 || m_IsOneShot)
        {
            if (m_AudioBank == nullptr)
            {
                Log::Warning("Can not load event. No bank assigned to audio source.");
                return;
            }

            String bankPath = ToString(m_AudioBank->GetPath());

            if (!m_Fmod->IsBankLoaded(bankPath) 
                && !m_Fmod->IsBankLoading(bankPath))
            {
                LoadBank(bankPath);
            }

            if (m_IsOneShot)
            {
                m_Fmod->PlayOneShot(m_EventName, audioSourceEntity);
                audioSourceEntity.RemoveComponent<AudioSourceComponent>();
                return;
            }

            m_EventHandle = m_Fmod->CreateSound(m_EventName, audioSourceEntity, true);
        }
        else
        {
            m_Fmod->PlaySound(m_EventHandle);
        }
    }

    void AudioPlayer::SetPaused(bool isPaused)
    {
        if (m_IsOneShot)
        {
            Log::Warning("Can not pause a one shot sound.");
            return;
        }

        m_Fmod->SetPause(m_EventHandle, isPaused);
    }

    void AudioPlayer::Stop()
    {
        if (m_IsOneShot)
        {
            Log::Warning("Can not stop a one shot sound.");
            return;
        }

        m_Fmod->StopSound(m_EventHandle);
    }

#pragma region Getters
    Ref<AudioBankAsset>& AudioPlayer::GetAudioBankAsset()
    {
        return m_AudioBank;
    }

    const String& AudioPlayer::GetEventName() const
    {
        return m_EventName;
    }

    bool AudioPlayer::GetPlayOnInit() const
    {
        return m_PlayOnInit;
    }

    bool AudioPlayer::GetIsOneShot() const
    {
        return m_IsOneShot;
    }
#pragma endregion


#pragma region Setters
    void AudioPlayer::SetAudioBankAsset(Ref<AudioBankAsset> audioBank)
    {
        m_AudioBank = audioBank;
    }

    void AudioPlayer::SetEventName(String eventName)
    {
        m_EventName = eventName;
    }

    void AudioPlayer::SetPlayOnInit(bool playOnInit)
    {
        m_PlayOnInit = playOnInit;
    }

    void AudioPlayer::SetIsOneShot(bool isOneShot)
    {
        m_IsOneShot = isOneShot;
    }
#pragma endregion

    void AudioPlayer::LoadBank(String& bankPath)
    {
        m_Fmod->LoadBank(bankPath);

        int index = bankPath.find('.');
        String bankFilePath = bankPath.left(index);
        String bankStringsPath = bankFilePath + ".strings.bank";
        m_Fmod->LoadBank(bankStringsPath);
    }
}

