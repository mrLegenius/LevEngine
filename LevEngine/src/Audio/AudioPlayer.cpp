#include "levpch.h"
#include "AudioPlayer.h"

#include "Audio/Audio.h"
#include "Assets/AudioBankAsset.h"
#include "Scene/Components/Audio/AudioSource.h"
#include "Scene/SceneManager.h"

namespace LevEngine
{
    AudioPlayer::AudioPlayer()
    {
        m_EventHandle = 0;
    }

    AudioPlayer::~AudioPlayer()
    {
        ReleaseSound();
    }

    void AudioPlayer::ReleaseSound()
    {
        if (m_EventHandle != 0)
        {
            Audio::ReleaseOneEvent(m_EventHandle);
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

            if (!Audio::IsBankLoaded(bankPath)
                && !Audio::IsBankLoading(bankPath))
            {
                LoadBank(bankPath);
            }

            if (m_IsOneShot)
            {
                Audio::PlayOneShot(m_EventName, audioSourceEntity);
                audioSourceEntity.RemoveComponent<AudioSourceComponent>();
                return;
            }

            m_EventHandle = Audio::CreateSound(m_EventName, audioSourceEntity, true);
        }
        else
        {
            Audio::PlaySound(m_EventHandle);
        }
    }

    void AudioPlayer::Stop()
    {
        if (m_IsOneShot)
        {
            Log::Warning("Can not stop a one shot sound.");
            return;
        }

        Audio::StopSound(m_EventHandle);
    }

#pragma region Getters and Setters
    bool AudioPlayer::IsPaused()
    {
        if (m_IsOneShot)
        {
            Log::Warning("Can not pause a one shot sound.");
            return false;
        }

        return Audio::IsPaused(m_EventHandle);
    }

    void AudioPlayer::SetPaused(bool isPaused)
    {
        if (m_IsOneShot)
        {
            Log::Warning("Can not pause a one shot sound.");
            return;
        }

        Audio::SetPause(m_EventHandle, isPaused);
    }


    Ref<AudioBankAsset>& AudioPlayer::GetAudioBankAsset()
    {
        return m_AudioBank;
    }

    void AudioPlayer::SetAudioBankAsset(Ref<AudioBankAsset> audioBank)
    {
        m_AudioBank = audioBank;
    }


    const String& AudioPlayer::GetEventName() const
    {
        return m_EventName;
    }

    void AudioPlayer::SetEventName(String eventName)
    {
        m_EventName = eventName;
    }


    bool AudioPlayer::GetPlayOnInit() const
    {
        return m_PlayOnInit;
    }

    void AudioPlayer::SetPlayOnInit(bool playOnInit)
    {
        m_PlayOnInit = playOnInit;
    }


    bool AudioPlayer::GetIsOneShot() const
    {
        return m_IsOneShot;
    }

    void AudioPlayer::SetIsOneShot(bool isOneShot)
    {
        m_IsOneShot = isOneShot;
    }
#pragma endregion


    void AudioPlayer::LoadBank(String& bankPath)
    {
        Audio::LoadBank(bankPath);

        int index = bankPath.find('.');
        String bankFilePath = bankPath.left(index);
        String bankStringsPath = bankFilePath + ".strings.bank";
        Audio::LoadBank(bankStringsPath);
    }
}

