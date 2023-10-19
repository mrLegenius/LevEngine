#pragma once
#include "Scene/Components/TypeParseTraits.h"
#include <entt/entt.hpp>

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

        static void OnComponentConstruct(entt::registry& registry, entt::entity entity);
        static void OnComponentDestroy(entt::registry& registry, entt::entity entity);
        void Init();
        bool IsInitialized() const;
        void ResetInit();

        void Play();
        void SetPaused(bool isPaused);
        void Stop();

        [[nodiscard]] bool GetPlayOnInit() const;
        void SetPlayOnInit(bool playOnInit);
        [[nodiscard]] bool GetIsOneShot() const;
        void SetIsOneShot(bool isOneShot);

        bool IsBankLoaded(String& bankPath);
        void LoadBank(String& bankPath);
        
        Ref<AudioBankAsset> audioBank;
        String eventName;
        bool playOnInit{};
        bool isOneShot{};

    private:
        Ref<LevFmod> m_Fmod;
        FMOD::Studio::EventInstance* m_EventInstance;
        bool m_IsInited{};
    };
}

