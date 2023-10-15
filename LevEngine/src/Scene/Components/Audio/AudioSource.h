#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace FMOD
{
    namespace Studio
    {
        class EventInstance;
    }
}

namespace LevEngine
{
    class LevFmod;
    class AudioBankAsset;

    REGISTER_PARSE_TYPE(AudioSourceComponent);

    struct AudioSourceComponent
    {
        AudioSourceComponent();
        ~AudioSourceComponent();

        void Init();
        bool IsInitialized() const;
        void ResetInit();

        void Play();
        void SetPaused(bool isPaused);
        void Stop();

        [[nodiscard]] bool GetPlayOnInit() const;
        void SetPlayOnInit(bool playOnInit);
        
        Ref<AudioBankAsset> audioBank;
        String eventName;
        bool playOnInit{};

    private:
        Ref<LevFmod> m_Fmod;
        FMOD::Studio::EventInstance* m_EventInstance;
        bool m_IsInited{};
    };
}

